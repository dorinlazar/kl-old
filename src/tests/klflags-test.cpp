#include "kl/klflags.h"
#include "kl/kl.h"

enum class TestEnum { Flag0, Flag1, Flag2 };

int main() {
  KLFlags<TestEnum> flags;

  CHECK(flags.value() == 0);
  CHECK(!flags.is_set(TestEnum::Flag0));
  CHECK(!flags.is_set(TestEnum::Flag1));
  CHECK(!flags.is_set(TestEnum::Flag2));

  flags.set_flag(TestEnum::Flag0);
  CHECK(flags.value() == 1);
  CHECK(flags.is_set(TestEnum::Flag0));
  CHECK(!flags.is_set(TestEnum::Flag1));
  CHECK(!flags.is_set(TestEnum::Flag2));

  flags.set_flag(TestEnum::Flag1);
  CHECK(flags.value() == 3);
  CHECK(flags.is_set(TestEnum::Flag0));
  CHECK(flags.is_set(TestEnum::Flag1));
  CHECK(!flags.is_set(TestEnum::Flag2));

  flags.set_flag(TestEnum::Flag2);
  CHECK(flags.value() == 7);
  CHECK(flags.is_set(TestEnum::Flag0));
  CHECK(flags.is_set(TestEnum::Flag1));
  CHECK(flags.is_set(TestEnum::Flag2));

  flags.clear_flag(TestEnum::Flag0);
  CHECK(flags.value() == 6);
  CHECK(!flags.is_set(TestEnum::Flag0));
  CHECK(flags.is_set(TestEnum::Flag1));
  CHECK(flags.is_set(TestEnum::Flag2));

  flags.clear_flag(TestEnum::Flag1);
  CHECK(flags.value() == 4);
  CHECK(!flags.is_set(TestEnum::Flag0));
  CHECK(!flags.is_set(TestEnum::Flag1));
  CHECK(flags.is_set(TestEnum::Flag2));

  flags.clear_flag(TestEnum::Flag2);
  CHECK(flags.value() == 0);
  CHECK(!flags.is_set(TestEnum::Flag0));
  CHECK(!flags.is_set(TestEnum::Flag1));
  CHECK(!flags.is_set(TestEnum::Flag2));

  flags.set_flags(TestEnum::Flag1, TestEnum::Flag2);
  CHECK(flags.value() == 6);
  CHECK(!flags.is_set(TestEnum::Flag0));
  CHECK(flags.is_set(TestEnum::Flag1));
  CHECK(flags.is_set(TestEnum::Flag2));

  flags.clear_flags(TestEnum::Flag0, TestEnum::Flag2);
  CHECK(flags.value() == 2);
  CHECK(!flags.is_set(TestEnum::Flag0));
  CHECK(flags.is_set(TestEnum::Flag1));
  CHECK(!flags.is_set(TestEnum::Flag2));

  return 0;
}
