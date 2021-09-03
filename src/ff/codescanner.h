#pragma once

#include "kl/kltext.h"

namespace kl {

// maybe at one point of time we'll have a more sophisticated code parser.
// but for the time being, let's do this in a simple way that doesn't care
// about #ifdefs and other shenanigans.
class SourceCodeScanner {
  List<Text> _local;
  List<Text> _system;
  bool _hasMain = false;

public:
  SourceCodeScanner(const kl::Text& filename);
  const List<Text>& localIncludes();
  const List<Text>& systemIncludes();
  bool hasMain();
};
} // namespace kl
