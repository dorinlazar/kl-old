#pragma once
#include "kl/klvalue.h"

class SystemFlags {
  struct SFImpl;
  kl::uptr<SFImpl> d;

public:
  SystemFlags(kl::Value* val);
  kl::List<kl::Text> cxxflags(const kl::List<kl::Text>& headers);
  kl::List<kl::Text> ldflags(const kl::List<kl::Text>& headers);
};
