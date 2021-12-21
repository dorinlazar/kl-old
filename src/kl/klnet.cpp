#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "klnet.h"
#include "klexcept.h"

using namespace kl;

TcpClient::TcpClient(const Text& server, uint16_t port) {
  std::string server_str = server.toString();
  addrinfo hints, *res;
  if (0 != ::getaddrinfo()) {
    throw IOException::currentStandardError();
  }

  _socket_id = socket();
  if (0 != ::connect()) {
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
