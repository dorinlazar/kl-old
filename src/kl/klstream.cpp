#include "klstream.h"
using namespace kl;

static Text s_notImplemented = "Not implemented by derived class"_t;

bool Stream::canRead() { return false; }
bool Stream::canWrite() { return false; }
bool Stream::canSeek() { return false; }
bool Stream::canTimeout() { return false; }

size_t Stream::length() { throw OperationNotSupported("Stream::length"_t, s_notImplemented); }
size_t Stream::position() { throw OperationNotSupported("Stream::position"_t, s_notImplemented); }
size_t Stream::readTimeout() { throw OperationNotSupported("Stream::readTimeout"_t, s_notImplemented); }
void Stream::setReadTimeout(size_t) { throw OperationNotSupported("Stream::setReadTimeout"_t, s_notImplemented); }
size_t Stream::writeTimeout() { throw OperationNotSupported("Stream::writeTimeout"_t, s_notImplemented); }
void Stream::setWriteTimeout(size_t) { throw OperationNotSupported("Stream::setWriteTimeout"_t, s_notImplemented); }
size_t Stream::read(std::span<uint8_t> where) { throw OperationNotSupported("Stream::read"_t, s_notImplemented); }
void Stream::write(std::span<uint8_t> what) { throw OperationNotSupported("Stream::write"_t, s_notImplemented); }
void Stream::write(const List<std::span<uint8_t>>& what) {
  throw OperationNotSupported("Stream::write"_t, s_notImplemented);
}
void Stream::seek(size_t offset) { throw OperationNotSupported("Stream::seek"_t, s_notImplemented); }
bool Stream::dataAvailable() { throw OperationNotSupported("Stream::dataAvailable"_t, s_notImplemented); }
void Stream::flush() { throw OperationNotSupported("Stream::flush"_t, s_notImplemented); }
void Stream::close() { throw OperationNotSupported("Stream::close"_t, s_notImplemented); }

StreamReader::StreamReader(Stream* stream) : _stream(stream), _offset(0), _readSize(0) {}
Stream* StreamReader::stream() const { return _stream; }
size_t StreamReader::read(std::span<uint8_t> where) {
  if (_offset >= _readSize) {
    return _stream->read(where);
  }
  auto bufsize = std::min(_readSize - _offset, where.size());
  _offset += bufsize;
  std::copy(_buffer.begin() + _offset, _buffer.begin() + _readSize, where);
  if (bufsize < where.size()) {
    return bufsize + _stream->read(where.subspan(bufsize));
  }
  return bufsize;
}

Text StreamReader::readLine() {
  TextChain tc;
  while (!_stream->endOfStream()) {
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
  while (!_stream->endOfStream()) {
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
