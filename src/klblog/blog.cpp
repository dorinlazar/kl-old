#include "blog.hpp"
#include "kl/kl.h"

namespace klblog {

Blog::Blog(std::shared_ptr<SystemSettings> sys) {
  if (sys->verbosity == VerbosityLevel::Verbose) {
    kl::log("Blog processing started");
  }
}

void Blog::process() const {}

} // namespace klblog
