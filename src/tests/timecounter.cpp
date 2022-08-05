#include "timecounter.h"

TimeCounter::TimeCounter(const kl::Text& op) : _start(kl::DateTime::now()), _op(op) {}
TimeCounter::~TimeCounter() { kl::log("Time spent {} {}", _op, kl::DateTime::now() - _start); }
