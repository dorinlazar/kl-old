#include "klexcept.h"
using namespace kl;

OperationNotSupported::OperationNotSupported(const Text& op, const Text& reason)
    : std::logic_error((op + ": " + reason).toText().toString()) {}

IOException::IOException(const Text& why) : _why(why) {}
IOException IOException::currentStandardError() { return IOException(strerror(errno)); }
const Text& IOException::what() { return _why; }
