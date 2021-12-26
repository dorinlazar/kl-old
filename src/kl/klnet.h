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
  bool canRead() override final;
  bool canWrite() override final;
  bool canSeek() override final;
  bool canTimeout() override final;

public: // properties
  size_t size() override final;
  size_t position() override final;

public: // operations
  bool dataAvailable() override final;
  bool endOfStream() override final;
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
  bool canRead() override;
  bool canWrite() override;
  bool canSeek() override;
  bool canTimeout() override;

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
