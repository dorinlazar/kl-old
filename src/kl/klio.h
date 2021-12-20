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
public:
  virtual ~Stream();

public: // capabilities
  virtual bool canRead();
  virtual bool canWrite();
  virtual bool canSeek();
  virtual bool canTimeout();

public: // properties
  virtual size_t size();
  virtual size_t position();

public: // operations
  virtual size_t read(std::span<uint8_t> where);
  virtual void write(std::span<uint8_t> what);
  virtual void write(const List<std::span<uint8_t>>& what);

  virtual void seek(size_t offset);
  virtual bool dataAvailable();
  virtual bool endOfStream();
  virtual void flush();

  virtual void close();
};

const size_t BASE_BUFFER_SIZE = 4096;
const size_t STREAM_BUFFER_SIZE =
    BASE_BUFFER_SIZE - sizeof(std::array<uint8_t, 16>) + 16 - sizeof(Stream*) - 2 * sizeof(size_t);

class StreamReader {
  Stream* _stream;
  size_t _offset, _readSize;
  std::array<uint8_t, STREAM_BUFFER_SIZE> _buffer;

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
};

enum class FileOpenMode { ReadOnly, WriteOnly, ReadWrite, AppendRW, TruncateRW };

class PosixFileStream : public Stream {
protected:
  int _fd = -1;
  bool _regular = false;

public:
  PosixFileStream(int fd);

public: // properties
  size_t size() override final;
  size_t position() override final;

public: // operations
  size_t read(std::span<uint8_t> where) override final;
  void write(std::span<uint8_t> what) override final;
  void write(const List<std::span<uint8_t>>& what) override final;

  void seek(size_t offset) override final;
  bool dataAvailable() override final;
  void flush() override final;
  bool endOfStream() override final;

  void close() override final;
};

class FileStream : public PosixFileStream {
  FileOpenMode _mode;

public:
  FileStream(const Text& filename, FileOpenMode mode);

public: // capabilities
  bool canRead() override final;
  bool canWrite() override final;
  bool canSeek() override final;
};

} // namespace kl
