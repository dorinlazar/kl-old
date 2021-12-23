#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "klnet.h"
#include "klexcept.h"

using namespace kl;

TcpClient::TcpClient(const Text& server, uint16_t port) {
  std::string server_str = server.toString();
  addrinfo hints = {.ai_flags = AI_ADDRCONFIG | AI_PASSIVE | AI_ALL | AI_NUMERICSERV,
                    .ai_family = AF_UNSPEC,
                    .ai_socktype = SOCK_STREAM,
                    .ai_protocol = 0};
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

  _socket_id = socket(candidate->ai_family, candidate->ai_socktype, IPPROTO_TCP);
  if (0 != ::connect(_socket_id, candidate->ai_addr, candidate->ai_addrlen)) {
    throw IOException::currentStandardError();
  }
}

TcpClient::~TcpClient() { close(); }
bool TcpClient::canRead() { return true; }
bool TcpClient::canWrite() { return true; }
bool TcpClient::canSeek() { return false; }
bool TcpClient::canTimeout() { return true; }
size_t TcpClient::size() { throw OperationNotSupported("TcpClient::size()", ""); }
size_t TcpClient::position() { throw OperationNotSupported("TcpClient::size()", ""); }
size_t TcpClient::read(std::span<uint8_t> where) {}
void TcpClient::write(std::span<uint8_t> what) {}
void TcpClient::write(const List<std::span<uint8_t>>& what) {}
void TcpClient::seek(size_t offset) {}
bool TcpClient::dataAvailable() {}
bool TcpClient::endOfStream() {}
void TcpClient::flush() {}

void TcpClient::close() {
  if (_socket_id > 0) {
    if (0 != ::shutdown(_socket_id, SHUT_RDWR)) {
      throw IOException::currentStandardError();
    }
    _socket_id = -1;
  }
}

TimeSpan TcpClient::readTimeout() {}
void TcpClient::setReadTimeout(TimeSpan) {}
TimeSpan TcpClient::writeTimeout() {}
void TcpClient::setWriteTimeout(TimeSpan) {}
