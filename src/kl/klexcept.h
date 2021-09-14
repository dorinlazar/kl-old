#pragma once

#include <exception>
#include <stdexcept>
#include "kltext.h"

namespace kl {
class OperationNotSupported : public std::logic_error {
public:
  OperationNotSupported(const Text& op, const Text& reason);
};

} // namespace kl
