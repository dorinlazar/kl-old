#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <poll.h>

#include "klio.h"
using namespace kl;

static Text s_notImplemented = "Not implemented by derived class"_t;

Stream::~Stream() { close(); }

bool Stream::canRead() { return false; }
bool Stream::canWrite() { return false; }
bool Stream::canSeek() { return false; }
bool Stream::canTimeout() { return false; }

size_t Stream::size() { throw OperationNotSupported("Stream::size"_t, s_notImplemented); }
size_t Stream::position() { throw OperationNotSupported("Stream::position"_t, s_notImplemented); }
size_t Stream::read(std::span<uint8_t>) { throw OperationNotSupported("Stream::read"_t, s_notImplemented); }
void Stream::write(std::span<uint8_t>) { throw OperationNotSupported("Stream::write"_t, s_notImplemented); }
void Stream::seek(size_t) { throw OperationNotSupported("Stream::seek"_t, s_notImplemented); }
bool Stream::dataAvailable() { throw OperationNotSupported("Stream::dataAvailable"_t, s_notImplemented); }
void Stream::flush() { throw OperationNotSupported("Stream::flush"_t, s_notImplemented); }
void Stream::close() {}
bool Stream::endOfStream() { return false; }

StreamReader::StreamReader(Stream* stream) : _stream(stream), _offset(0), _readSize(0) {}
Stream* StreamReader::stream() const { return _stream; }
size_t StreamReader::read(std::span<uint8_t> where) {
  if (_offset >= _readSize) {
    return _stream->read(where);
  }
  auto bufsize = std::min(_readSize - _offset, where.size());
  _offset += bufsize;
  std::copy(_buffer.begin() + _offset, _buffer.begin() + _readSize, where.data());
  if (bufsize < where.size()) {
    return bufsize + _stream->read(where.subspan(bufsize));
  }
  return bufsize;
}

Text StreamReader::readLine() {
  TextChain tc;
  while (_offset < _readSize || !_stream->endOfStream()) {
    if (_offset >= _readSize) {
      _offset = 0;
      _readSize = _stream->read(_buffer);
    }
    auto _startOffset = _offset;
    for (; _offset < _readSize; _offset++) {
      if (_buffer[_offset] == '\n') { // TODO identify newlines based on encoding.
        tc.add(Text((const char*)_buffer.begin() + _startOffset, _offset - _startOffset));
        _offset++;
        return tc.toText();
      }
    }
    tc.add(Text((const char*)_buffer.begin() + _startOffset, _offset - _startOffset));
  }
  return tc.toText();
}

Text StreamReader::readAll() {
  TextChain tc;
  while (_offset < _readSize || !_stream->endOfStream()) {
    if (_offset >= _readSize) {
      _offset = 0;
      _readSize = _stream->read(_buffer);
    }
    tc.add(Text((const char*)_buffer.begin() + _offset, _readSize - _offset));
    _offset = _readSize;
  }
  return tc.toText();
}

bool StreamReader::endOfStream() { return _stream->endOfStream(); }

StreamWriter::StreamWriter(Stream* stream) : _stream(stream) {}
Stream* StreamWriter::stream() const { return _stream; }
void StreamWriter::write(std::span<uint8_t> what) { _stream->write(what); }
void StreamWriter::write(const Text& what) { _stream->write(what.toRawData()); }
void StreamWriter::writeLine(const Text& what) {
  static char eol[] = "\n";
  _stream->write(what.toRawData());
  _stream->write(std::span<uint8_t>((uint8_t*)&eol[0], 1));
}
void StreamWriter::write(const TextChain& what) { _stream->write(what.toText().toRawData()); }
void StreamWriter::flush() { _stream->flush(); }

PosixFileStream::PosixFileStream(int fd) : _fd(fd) {
  struct stat statbuf;
  if (::fstat(_fd, &statbuf) == 0) {
    _regular = S_ISREG(statbuf.st_mode);
  }
}

static int openFile(const Text& filename, FileOpenMode mode) {
  int flags = 0;
  switch (mode) {
  case FileOpenMode::ReadOnly: flags = O_RDONLY; break;
  case FileOpenMode::WriteOnly: flags = O_WRONLY | O_CREAT; break;
  case FileOpenMode::ReadWrite: flags = O_RDWR | O_CREAT; break;
  case FileOpenMode::AppendRW: flags = O_RDWR | O_APPEND | O_CREAT; break;
  case FileOpenMode::TruncateRW: flags = O_RDWR | O_TRUNC | O_CREAT; break;
  }
  return ::open(std::string(filename.toView()).c_str(), flags);
}

FileStream::FileStream(const Text& filename, FileOpenMode mode)
    : PosixFileStream(openFile(filename, mode)), _mode(mode) {}

bool FileStream::canRead() { return _mode != FileOpenMode::WriteOnly; }
bool FileStream::canWrite() { return _mode != FileOpenMode::ReadOnly; }
bool FileStream::canSeek() { return true; }

size_t PosixFileStream::size() {
  if (_regular) {
    struct stat statbuf;
    if (::fstat(_fd, &statbuf) != 0) [[unlikely]] {
      throw IOException::currentStandardError();
    };
    return statbuf.st_size;
  }
  return 0;
}

size_t PosixFileStream::position() {
  if (_regular) {
    auto pos = ::lseek(_fd, 0, SEEK_CUR);
    if (pos < 0) [[unlikely]] {
      throw IOException::currentStandardError();
    }
    return pos;
  }
  return 0;
}

size_t PosixFileStream::read(std::span<uint8_t> where) {
  auto res = ::read(_fd, where.data(), where.size());
  if (res < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
  return res;
}

void PosixFileStream::write(std::span<uint8_t> what) {
  // TODO: write everything!
  size_t offset = 0;
  while (offset < what.size()) {
    auto bytes_written = ::write(_fd, what.data() + offset, what.size() - offset);
    if (bytes_written < 0) [[unlikely]] {
      throw IOException::currentStandardError();
    }
    offset += bytes_written;
  }
}

void PosixFileStream::seek(size_t offset) {
  if (lseek(_fd, offset, SEEK_SET) < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
}

bool PosixFileStream::dataAvailable() {
  if (_regular) {
    return position() < size();
  }
  auto pfd = pollfd{.fd = _fd, .events = 0, .revents = 0};
  return poll(&pfd, 1, 0) > 0;
}

bool PosixFileStream::endOfStream() {
  if (_regular) [[likely]] {
    return position() >= size();
  }
  throw OperationNotSupported("End of stream", "non-regular file");
}

void PosixFileStream::flush() {
  if (fdatasync(_fd) < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
}

void PosixFileStream::close() {
  if (_fd >= 0) {
    ::close(_fd);
    _fd = -1;
  }
}

PosixFileStream::~PosixFileStream() { close(); }
