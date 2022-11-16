#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <poll.h>

#include "klio.h"
using namespace kl;

static Text s_notImplemented = "Not implemented by derived class"_t;

Stream::~Stream() { close(); }

bool Stream::can_read() { return false; }
bool Stream::can_write() { return false; }
bool Stream::can_seek() { return false; }
bool Stream::can_timeout() { return false; }

size_t Stream::size() { throw OperationNotSupported("Stream::size"_t, s_notImplemented); }
size_t Stream::position() { throw OperationNotSupported("Stream::position"_t, s_notImplemented); }
size_t Stream::read(std::span<uint8_t>) { throw OperationNotSupported("Stream::read"_t, s_notImplemented); }
void Stream::write(std::span<uint8_t>) { throw OperationNotSupported("Stream::write"_t, s_notImplemented); }
void Stream::seek(size_t) { throw OperationNotSupported("Stream::seek"_t, s_notImplemented); }
bool Stream::data_available() { throw OperationNotSupported("Stream::data_available"_t, s_notImplemented); }
void Stream::flush() { throw OperationNotSupported("Stream::flush"_t, s_notImplemented); }
void Stream::close() {}
bool Stream::end_of_stream() { return false; }

StreamReader::StreamReader(Stream* stream) : m_stream(stream) {}
Stream* StreamReader::stream() const { return m_stream; }
size_t StreamReader::read(std::span<uint8_t> where) {
  if (m_offset >= m_read_size) { // skip the buffer
    return m_stream->read(where);
  }
  auto bufsize = std::min(m_read_size - m_offset, where.size());
  std::copy(m_buffer.data() + m_offset, m_buffer.data() + m_offset + bufsize, where.data());
  m_offset += bufsize;
  if (bufsize < where.size()) {
    m_offset = 0;
    m_read_size = 0;
    return bufsize + m_stream->read(where.subspan(bufsize));
  }
  return bufsize;
}

Text StreamReader::read_line() {
  TextChain tc;
  while (true) {
    if (m_offset >= m_read_size) {
      m_offset = 0;
      m_read_size = m_stream->read(m_buffer);
      if (m_read_size == 0) {
        break;
      }
    }
    auto _startOffset = m_offset;
    for (; m_offset < m_read_size; m_offset++) {
      if (m_buffer[m_offset] == '\n') { // TODO identify newlines based on encoding.
        tc.add(Text((const char*)m_buffer.data() + _startOffset, m_offset - _startOffset));
        m_offset++;
        return tc.toText();
      }
    }
    tc.add(Text((const char*)m_buffer.data() + _startOffset, m_offset - _startOffset));
  }
  return tc.toText();
}

Text StreamReader::read_all() {
  TextChain tc;
  while (true) {
    if (m_offset >= m_read_size) {
      m_offset = 0;
      m_read_size = m_stream->read(m_buffer);
      if (m_read_size == 0) {
        break;
      }
    }
    tc.add(Text((const char*)m_buffer.data() + m_offset, m_read_size - m_offset));
    m_read_size = 0;
    m_offset = 0;
  }
  return tc.toText();
}

bool StreamReader::end_of_stream() { return m_stream->end_of_stream(); }

StreamWriter::StreamWriter(Stream* stream) : _stream(stream) {}
Stream* StreamWriter::stream() const { return _stream; }
void StreamWriter::write(std::span<uint8_t> what) { _stream->write(what); }
void StreamWriter::write(const Text& what) { _stream->write(what.toRawData()); }
void StreamWriter::write_line(const Text& what) {
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
  if (flags & O_CREAT) {
    return ::open(std::string(filename.toView()).c_str(), flags, 0600);
  }
  return ::open(std::string(filename.toView()).c_str(), flags);
}

FileStream::FileStream(const Text& filename, FileOpenMode mode)
    : PosixFileStream(openFile(filename, mode)), _mode(mode) {}

bool FileStream::can_read() { return _mode != FileOpenMode::WriteOnly; }
bool FileStream::can_write() { return _mode != FileOpenMode::ReadOnly; }
bool FileStream::can_seek() { return true; }

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

bool PosixFileStream::data_available() {
  if (_regular) {
    return position() < size();
  }
  auto pfd = pollfd{.fd = _fd, .events = 0, .revents = 0};
  return poll(&pfd, 1, 0) > 0;
}

bool PosixFileStream::end_of_stream() {
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

int PosixFileStream::file_descriptor() { return _fd; }
