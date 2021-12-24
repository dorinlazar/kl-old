#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

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
  Defer{[addresses]() { freeaddrinfo(addresses); }};

  addrinfo* candidate = addresses;
  for (auto p = addresses; p != nullptr; p = p->ai_next) {
    candidate = p;
    if (p->ai_family == AF_INET6) {
      break;
    }
  }

  int socketid = socket(candidate->ai_family, candidate->ai_socktype, IPPROTO_TCP);
  if (0 != ::connect(socketid, candidate->ai_addr, candidate->ai_addrlen)) {
    throw IOException::currentStandardError();
  }
  return socketid;
}

TcpClient::TcpClient(const Text& server, uint16_t port) : PosixFileStream(connect_to_server(server, port)) {}

bool TcpClient::canRead() { return true; }
bool TcpClient::canWrite() { return true; }
bool TcpClient::canSeek() { return false; }
bool TcpClient::canTimeout() { return true; }
size_t TcpClient::size() { throw OperationNotSupported("TcpClient::size()", ""); }
size_t TcpClient::position() { throw OperationNotSupported("TcpClient::size()", ""); }

bool TcpClient::dataAvailable() {
  struct pollfd pfd = {.fd = _fd, .events = POLLIN, .revents = 0};
  return ::poll(&pfd, 1, 0) > 0;
}

bool TcpClient::endOfStream() { throw OperationNotSupported("TcpClient::endOfStream()", ""); }
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
