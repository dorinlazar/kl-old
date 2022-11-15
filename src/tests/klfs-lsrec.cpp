#include "kl/klfs.h"

int main() {
  kl::FileSystem::navigate_tree(".", [](const kl::FileSystemEntryInfo& fi) {
    kl::log("{} {} {}", fi.type == kl::FileType::Directory ? "D" : "F", fi.lastWrite, fi.path.full_path());
    return fi.path.full_path()[0] == '.' ? kl::NavigateInstructions::Skip : kl::NavigateInstructions::Continue;
  });
}
