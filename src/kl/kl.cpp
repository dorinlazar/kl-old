#include "kl.h"

using namespace kl;
Defer::Defer(std::function<void(void)> call) : _call(call) {}
Defer::~Defer() { _call(); }
