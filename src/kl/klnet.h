#pragma once
#include "klio.h"
#include "kltime.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

namespace kl {

enum class SocketAddressFamily : sa_family_t { UNIX = AF_UNIX, IPv4 = AF_INET, IPv6 = AF_INET6 };
enum class SocketType : int {
  STREAM = SOCK_STREAM,
  DATAGRAM = SOCK_DGRAM,
  RAW = SOCK_RAW,
  SEQUENTIALPACKET = SOCK_SEQPACKET
};

struct IPv4EndPoint {
  SocketAddressFamily _family = SocketAddressFamily::IPv4;
  uint16_t _port;
  uint32_t _address;
  uint8_t _padding[8];

public:
  IPv4EndPoint(uint32_t address, uint16_t port);
  IPv4EndPoint(const Text& address, uint16_t port);
  IPv4EndPoint(const Text& address);

  void port(uint16_t);
  uint16_t port() const;

  void address(uint32_t);
  uint32_t address() const;
};

static_assert(sizeof(sockaddr_in) == sizeof(IPv4EndPoint));

class Socket : public PosixFileStream {

public:
  TimeSpan readTimeout();
  void setReadTimeout(TimeSpan);
  TimeSpan writeTimeout();
  void setWriteTimeout(TimeSpan);
};
} // namespace kl
