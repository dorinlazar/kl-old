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
void Stream::seek(size_t offset) { throw OperationNotSupported("Stream::seek"_t, s_notImplemented); }
bool Stream::dataAvailable() { throw OperationNotSupported("Stream::dataAvailable"_t, s_notImplemented); }
void Stream::flush() { throw OperationNotSupported("Stream::flush"_t, s_notImplemented); }
void Stream::close() { throw OperationNotSupported("Stream::close"_t, s_notImplemented); }
