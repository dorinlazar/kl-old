#include "kltimecounter.h"

namespace kl {
TimeCounter::TimeCounter(const kl::Text& op) : _start(kl::DateTime::now()), _op(op) {}
TimeCounter::~TimeCounter() { kl::log("{} done. Time spent: {}", _op, kl::DateTime::now() - _start); }
} // namespace kl
