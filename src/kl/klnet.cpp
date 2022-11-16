#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <openssl/ssl.h>
#include <arpa/inet.h>

#include "klnet.h"
#include "klexcept.h"

using namespace kl;

static int connect_to_server(const Text& server, uint16_t port) {
  std::string server_str = server.toString();
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_ALL | AI_NUMERICSERV;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  addrinfo* addresses = nullptr;
  auto callres = ::getaddrinfo(server.toString().c_str(), std::to_string(port).c_str(), &hints, &addresses);
  if (0 != callres) {
    throw IOException(gai_strerror(callres));
  }

  addrinfo* candidate = addresses;
  char buffer[128];
  for (auto p = addresses; p != nullptr; p = p->ai_next) {
    candidate = p;
    ::inet_ntop(p->ai_family, p->ai_addr, buffer, 128);
    if (p->ai_family == AF_INET6) {
      break;
    }
  }

  int socketid = socket(candidate->ai_family, candidate->ai_socktype, candidate->ai_protocol);
  auto res = ::connect(socketid, candidate->ai_addr, candidate->ai_addrlen);
  freeaddrinfo(addresses);
  if (0 != res) {
    throw IOException::currentStandardError();
  }
  log("Connected...");
  return socketid;
}

TcpClient::TcpClient(const Text& server, uint16_t port) : PosixFileStream(connect_to_server(server, port)) {}

bool TcpClient::can_read() { return true; }
bool TcpClient::can_write() { return true; }
bool TcpClient::can_seek() { return false; }
bool TcpClient::can_timeout() { return true; }
size_t TcpClient::size() { throw OperationNotSupported("TcpClient::size()", ""); }
size_t TcpClient::position() { throw OperationNotSupported("TcpClient::size()", ""); }

bool TcpClient::data_available() {
  struct pollfd pfd = {.fd = _fd, .events = POLLIN, .revents = 0};
  return ::poll(&pfd, 1, 0) > 0;
}

bool TcpClient::end_of_stream() { throw OperationNotSupported("TcpClient::end_of_stream()", ""); }
void TcpClient::flush() { throw OperationNotSupported("TcpClient::flush()", ""); }

TimeSpan TcpClient::readTimeout() {
  struct timeval timeout;
  socklen_t length = sizeof(timeout);
  if (getsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, &length) < 0) {
    return {};
  }
  return TimeSpan::fromTimeval(timeout);
}

void TcpClient::setReadTimeout(TimeSpan ts) {
  struct timeval timeout = ts.timeval();
  setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

TimeSpan TcpClient::writeTimeout() {
  struct timeval timeout;
  socklen_t length = sizeof(timeout);
  if (getsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, &length) < 0) {
    return {};
  }
  return TimeSpan::fromTimeval(timeout);
}

void TcpClient::setWriteTimeout(TimeSpan ts) {
  struct timeval timeout = ts.timeval();
  setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

class SSLHandler {
  SSL_CTX* ctx;
  SSLHandler() { ctx = SSL_CTX_new(SSLv23_method()); }
  ~SSLHandler() {
    SSL_CTX_free(ctx);
    ctx = nullptr;
  }

  SSL* createSSL() { return SSL_new(ctx); }

public:
  static SSL* create() {
    static SSLHandler libhandler;
    return libhandler.createSSL();
  }
};

struct SslClient::SslClientImpl {
  TcpClient client;
  SSL* sslHandler;
  SslClientImpl(const Text& server, uint16_t port) : client(server, port), sslHandler(SSLHandler::create()) {
    SSL_set_fd(sslHandler, client.fileDescriptor());
    auto res = SSL_connect(sslHandler);
    if (res <= 0) {
      throw IOException("SSL Connect error "_t + std::to_string(SSL_get_error(sslHandler, res)));
    }
  }

  ~SslClientImpl() { close(); }

  void close() {
    if (sslHandler) {
      SSL_shutdown(sslHandler);
      sslHandler = nullptr;
      client.close();
    }
  }

  size_t read(std::span<uint8_t> where) {
    auto res = SSL_read(sslHandler, where.data(), where.size());
    if (res <= 0) {
      auto err = SSL_get_error(sslHandler, res);
      if (err == SSL_ERROR_ZERO_RETURN) {
        return 0;
      }
      throw IOException("SSL Read error "_t + std::to_string(err));
    }
    return res;
  }

  void write(std::span<uint8_t> what) {
    auto res = SSL_write(sslHandler, what.data(), what.size());
    if (res <= 0) {
      throw IOException("SSL Write error "_t + std::to_string(SSL_get_error(sslHandler, res)));
    }
  }
};

SslClient::SslClient(const Text& server, uint16_t port)
    : _impl(std::make_unique<SslClient::SslClientImpl>(server, port)) {}

SslClient::~SslClient() {}

bool SslClient::can_read() { return true; }
bool SslClient::can_write() { return true; }
bool SslClient::can_seek() { return false; }
bool SslClient::can_timeout() { return true; }
size_t SslClient::read(std::span<uint8_t> where) { return _impl->read(where); }
void SslClient::write(std::span<uint8_t> what) { _impl->write(what); }

void SslClient::close() { _impl->close(); }

TimeSpan SslClient::readTimeout() { return _impl->client.readTimeout(); }
void SslClient::setReadTimeout(TimeSpan ts) { _impl->client.setReadTimeout(ts); }
TimeSpan SslClient::writeTimeout() { return _impl->client.writeTimeout(); }
void SslClient::setWriteTimeout(TimeSpan ts) { _impl->client.setWriteTimeout(ts); }
