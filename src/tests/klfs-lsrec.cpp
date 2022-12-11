#include "kl/klfs.h"

int main() {
  kl::FileSystem::navigate_tree(".", [](const kl::FileSystemEntryInfo& fi) {
    kl::log("{} {} {}", fi.type == kl::FileType::Directory ? "D" : "F", fi.last_write, fi.path.fullPath());
    return fi.path.fullPath()[0] == '.' ? kl::NavigateInstructions::Skip : kl::NavigateInstructions::Continue;
  });
}
