#include "kl/klfs.h"

int main() {
  kl::navigateTree(".", [](const kl::FileInfo& fi) {
    kl::log(fi.type == kl::FileType::Directory ? "D" : "F", fi.lastWrite, fi.path.fullPath());
    return fi.path.fullPath()[0] == '.' ? kl::NavigateInstructions::Skip : kl::NavigateInstructions::Continue;
  });
}
