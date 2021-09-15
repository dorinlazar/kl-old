#pragma once

#include <stddef.h>
#include <vector>
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
  virtual bool endOfStream();
  virtual void flush();

  virtual void close();
};

const size_t BASE_BUFFER_SIZE = 1024;
const size_t STREAM_BUFFER_SIZE =
    BASE_BUFFER_SIZE - sizeof(std::array<uint8_t, 16>) + 16 - sizeof(Stream*) - 2 * sizeof(size_t);

class StreamReader {
  std::array<uint8_t, STREAM_BUFFER_SIZE> _buffer;
  Stream* _stream;
  size_t _offset, _readSize;

public:
  StreamReader(Stream* stream);
  Stream* stream() const;
  size_t read(std::span<uint8_t> where);
  Text readLine();
  Text readAll();
  bool endOfStream();
};

static_assert(sizeof(StreamReader) == BASE_BUFFER_SIZE);

class StreamWriter {
  Stream* _stream;

public:
  StreamWriter(Stream* stream);
  Stream* stream() const;
  void write(std::span<uint8_t> what);
  void write(const Text& what);
  void writeLine(const Text& what);
  void write(const TextChain& what);
  void flush();
  bool endOfStream();
};

} // namespace kl
