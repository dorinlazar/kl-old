#pragma once

#include <exception>
#include <stdexcept>
#include "kltext.h"

namespace kl {
class OperationNotSupported : public std::logic_error {
public:
  OperationNotSupported(const Text& op, const Text& reason);
};

class InvalidInputData : public std::logic_error {
public:
  InvalidInputData(const Text& what, const Text& expected);
};

class IOException {
  Text _why;

public:
  IOException(const Text& why);
  const Text& what();
  static IOException currentStandardError();
};

} // namespace kl
