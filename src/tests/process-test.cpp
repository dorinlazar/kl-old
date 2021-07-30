#include <stdlib.h>
#include "kl/klprocess.h"

void run_test() {
  kl::Process p({"ls"_t, "-l"_t, "-a"_t});
  kl::Process p2({"ls"_t, "-l"_t, "-a"_t, "/"_t});

  p.spawn();
  p2.spawn();
}

int main() {
  run_test();
  kl::log("processes finished");
  return 0;
}
