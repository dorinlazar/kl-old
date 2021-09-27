#pragma once
#include "kl/klvalue.h"
#include "textscanner.h"

namespace kl {
class PoorConfig {
  PValue _value;

public:
  PoorConfig(const Text& filename);
  PValue top() const;

public:
  static PValue parse(const Text& fragment, char split = ':');
  static PValue parse(TextScanner& scanner, char split = ':');
};
} // namespace kl
