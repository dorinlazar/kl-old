#pragma once
#include "kl/kltime.h"

namespace kl {
class TimeCounter {
  kl::DateTime _start;
  kl::Text _op;

public:
  TimeCounter(const Text& operation);
  ~TimeCounter();
};
}