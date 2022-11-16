#include "kl/klio.h"
#include "kl/kl.h"

void test_basic() {
  kl::FileStream fs("data/klblog/blog.config", kl::FileOpenMode::ReadOnly);
  kl::StreamReader sr(&fs);
  auto next = sr.read_line();
  CHECKST(next == "# Don't modify this heading");
  next = sr.read_line();
  CHECKST(next == "# Let's keep it .yml for the sake of syntax highlights");
  kl::log("Basic test ok");
}

int main() {
  test_basic();
  return 0;
}
