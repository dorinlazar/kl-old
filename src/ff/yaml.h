#pragma once

#include "kl/klvalue.h"

namespace kl {
class Yaml {
public:
  static PValue readFile(const Text& name);
};
} // namespace kl
