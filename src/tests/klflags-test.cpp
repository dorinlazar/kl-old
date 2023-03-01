#include "kl/klflags.h"
#include "kl/kl.h"

enum class TestEnum { Flag0, Flag1, Flag2 };

int main() {
  KLFlags<TestEnum> flags;
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  flags.set_flag(TestEnum::Flag0);
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  flags.set_flag(TestEnum::Flag1);
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  flags.set_flag(TestEnum::Flag2);
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  flags.clear_flag(TestEnum::Flag0);
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  flags.clear_flag(TestEnum::Flag1);
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  flags.clear_flag(TestEnum::Flag2);
  kl::log("Flags is set? {}", flags.is_set(TestEnum::Flag1) ? "true" : "false");
  return 0;
}