#include "klexcept.h"
using namespace kl;

OperationNotSupported::OperationNotSupported(const Text& op, const Text& reason)
    : std::logic_error(("Operation not supported"_t + op + ": " + reason).toText().toString()) {}
InvalidInputData::InvalidInputData(const Text& received, const Text& expected)
    : std::logic_error(("Invalid input data: [" + received + "], expected: " + expected).toText().toString()) {}

IOException::IOException(const Text& why) : _why(why) {}
IOException IOException::currentStandardError() { return IOException(strerror(errno)); }
const Text& IOException::what() { return _why; }
