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

class Socket : public Stream {
public:
  bool canRead() override final;
  bool canWrite() override final;
  bool canTimeout() override final;

public:
  TimeSpan readTimeout();
  void setReadTimeout(TimeSpan);
  TimeSpan writeTimeout();
  void setWriteTimeout(TimeSpan);

public:
  size_t read(std::span<uint8_t> where) override final;
  void write(std::span<uint8_t> what) override final;
  void write(const List<std::span<uint8_t>>& what) override final;

  bool dataAvailable() override final;
  void flush() override final;

  void close() override final;
};
} // namespace kl
