#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

#include "klnet.h"
#include "klexcept.h"

using namespace kl;

static int open_file_descriptor(const Text& server, uint16_t port) {
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

TcpClient::TcpClient(const Text& server, uint16_t port) : PosixFileStream(open_file_descriptor(server, port)) {}

TcpClient::~TcpClient() { close(); }
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

TimeSpan TcpClient::readTimeout() {}
void TcpClient::setReadTimeout(TimeSpan) {}
TimeSpan TcpClient::writeTimeout() {}
void TcpClient::setWriteTimeout(TimeSpan) {}
