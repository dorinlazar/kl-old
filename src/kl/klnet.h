#pragma once
#include "klio.h"
#include "kltime.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

namespace kl {

class TcpClient : public PosixFileStream {
public:
  TcpClient(const Text& server, uint16_t port);

public: // capabilities
  bool can_read() override final;
  bool can_write() override final;
  bool can_seek() override final;
  bool can_timeout() override final;

public: // properties
  size_t size() override final;
  size_t position() override final;

public: // operations
  bool data_available() override final;
  bool end_of_stream() override final;
  void flush() override final;

public:
  TimeSpan readTimeout();
  void setReadTimeout(TimeSpan);
  TimeSpan writeTimeout();
  void setWriteTimeout(TimeSpan);
};

class SslClient final : public Stream {
  struct SslClientImpl;
  std::unique_ptr<SslClientImpl> _impl;

public:
  SslClient(const Text& server, uint16_t port);
  virtual ~SslClient();

public: // capabilities
  bool can_read() override;
  bool can_write() override;
  bool can_seek() override;
  bool can_timeout() override;

public: // operations
  size_t read(std::span<uint8_t> where) override;
  void write(std::span<uint8_t> what) override;

  void close() override;

public:
  TimeSpan readTimeout();
  void setReadTimeout(TimeSpan);
  TimeSpan writeTimeout();
  void setWriteTimeout(TimeSpan);
};

class TcpServer {
public:
  TcpServer(uint16_t port);
  ~TcpServer();

  std::unique_ptr<Stream> accept();
};
} // namespace kl
