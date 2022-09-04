#pragma once
#include "kl/kltime.h"

class TimeCounter {
  kl::DateTime _start;
  kl::Text _op;

public:
  TimeCounter(const kl::Text& operation);
  ~TimeCounter();
};
