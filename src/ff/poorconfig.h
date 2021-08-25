#pragma once
#include "kl/klvalue.h"

namespace kl {
class PoorConfig {
public:
  static PValue parse(const Text& fragment, char split = ':');
};
} // namespace kl
