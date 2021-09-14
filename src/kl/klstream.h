#pragma once

#include <stddef.h>
#include <memory>
#include <span>
#include "kltext.h"
#include "klexcept.h"

namespace kl {

// Inspired by System.IO.Stream: https://docs.microsoft.com/en-us/dotnet/api/system.io.stream?view=net-5.0
class Stream {
public: // capabilities
  virtual bool canRead();
  virtual bool canWrite();
  virtual bool canSeek();
  virtual bool canTimeout();

public: // properties
  virtual size_t length();
  virtual size_t position();
  virtual size_t readTimeout();
  virtual void setReadTimeout(size_t);
  virtual size_t writeTimeout();
  virtual void setWriteTimeout(size_t);

public: // operations
  virtual size_t read(std::span<uint8_t> where);
  virtual void write(std::span<uint8_t> what);

  virtual void seek(size_t offset);
  virtual bool dataAvailable();
  virtual void flush();

  virtual void close();
};

class StreamReader {
  struct StreamReaderInternals;
  std::unique_ptr<StreamReaderInternals> _impl;

public:
  StreamReader(Stream* stream);
  size_t read(std::span<uint8_t> where);
  Text readLine();
  Text readAll();
  bool endOfStream();
};

class StreamWriter {
  struct StreamWriterInternals;
  std::unique_ptr<StreamWriterInternals> _impl;

public:
  StreamWriter(Stream* stream);
  void write(std::span<uint8_t> what);
  void write(const Text& what);
  void writeLine(const Text& what);
  void write(const TextChain& what);
  void flush();
  bool endOfStream();
};

} // namespace kl
