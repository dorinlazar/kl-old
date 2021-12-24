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
  Stream() = default;
  Stream(const Stream&) = delete;
  Stream(Stream&&) = delete;
  Stream& operator=(const Stream&) = delete;
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
  virtual ~PosixFileStream();

public: // properties
  size_t size() override;
  size_t position() override;

public: // operations
  size_t read(std::span<uint8_t> where) override;
  void write(std::span<uint8_t> what) override;

  void seek(size_t offset) override;
  bool dataAvailable() override;
  void flush() override;
  bool endOfStream() override;

  void close() override;
};

class FileStream final : public PosixFileStream {
  FileOpenMode _mode;

public:
  FileStream(const Text& filename, FileOpenMode mode);

public: // capabilities
  bool canRead() override final;
  bool canWrite() override final;
  bool canSeek() override final;
};

} // namespace kl
