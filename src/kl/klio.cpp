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
void Stream::write(const List<std::span<uint8_t>>&) {
  throw OperationNotSupported("Stream::write"_t, s_notImplemented);
}
void Stream::seek(size_t) { throw OperationNotSupported("Stream::seek"_t, s_notImplemented); }
bool Stream::dataAvailable() { throw OperationNotSupported("Stream::dataAvailable"_t, s_notImplemented); }
void Stream::flush() { throw OperationNotSupported("Stream::flush"_t, s_notImplemented); }
void Stream::close() {}

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
  _stream->write({what.toRawData(), {std::span<uint8_t>((uint8_t*)&eol[0], 1)}});
}
void StreamWriter::write(const TextChain& what) {
  _stream->write(what.chain().transform<std::span<uint8_t>>([](const Text& t) { return t.toRawData(); }));
}
void StreamWriter::flush() { _stream->flush(); }

FileStream::FileStream(const Text& filename, FileOpenMode mode) : _mode(mode) {
  int flags = 0;
  switch (mode) {
  case FileOpenMode::ReadOnly: flags = O_RDONLY; break;
  case FileOpenMode::WriteOnly: flags = O_WRONLY | O_CREAT; break;
  case FileOpenMode::ReadWrite: flags = O_RDWR | O_CREAT; break;
  case FileOpenMode::AppendRW: flags = O_RDWR | O_APPEND | O_CREAT; break;
  case FileOpenMode::TruncateRW: flags = O_RDWR | O_TRUNC | O_CREAT; break;
  }
  _fd = ::open(std::string(filename.toView()).c_str(), flags);
  struct stat statbuf;
  if (::fstat(_fd, &statbuf) != 0) [[unlikely]] {
    throw IOException::currentStandardError();
  };
  _regular = S_ISREG(statbuf.st_mode);
}

bool FileStream::canRead() { return _mode != FileOpenMode::WriteOnly; }
bool FileStream::canWrite() { return _mode != FileOpenMode::ReadOnly; }
bool FileStream::canSeek() { return true; }

size_t FileStream::size() {
  struct stat statbuf;
  if (::fstat(_fd, &statbuf) != 0) [[unlikely]] {
    throw IOException::currentStandardError();
  };
  return statbuf.st_size;
}
size_t FileStream::position() {
  auto pos = ::lseek(_fd, 0, SEEK_CUR);
  if (pos < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
  return pos;
}
size_t FileStream::read(std::span<uint8_t> where) {
  auto res = ::read(_fd, where.data(), where.size());
  if (res < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
  return res;
}
void FileStream::write(std::span<uint8_t> what) {
  if (::write(_fd, what.data(), what.size()) < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
}
void FileStream::write(const List<std::span<uint8_t>>& what) {
  std::vector<iovec> vectors(what.size());
  for (size_t i = 0; i < what.size(); i++) {
    vectors[i].iov_base = (void*)what[i].data();
    vectors[i].iov_len = what[i].size();
  }
  if (writev(_fd, vectors.data(), vectors.size()) < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
}

void FileStream::seek(size_t offset) {
  if (lseek(_fd, offset, SEEK_SET) < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
}
bool FileStream::dataAvailable() {
  if (_regular) [[likely]] {
    return position() < size();
  }
  auto pfd = pollfd{.fd = _fd, .events = 0, .revents = 0};
  return poll(&pfd, 1, 0) > 0;
}
bool FileStream::endOfStream() {
  if (_regular) [[likely]] {
    return position() >= size();
  }
  throw OperationNotSupported("End of stream", "non-regular file");
}
void FileStream::flush() {
  if (fdatasync(_fd) < 0) [[unlikely]] {
    throw IOException::currentStandardError();
  }
}

void FileStream::close() {
  if (_fd >= 0) {
    ::close(_fd);
    _fd = -1;
  }
}
