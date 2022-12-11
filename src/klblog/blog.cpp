#include "blog.hpp"
#include "kl/kl.h"
#include "kl/klfs.h"

namespace klblog {

Blog::Blog(std::shared_ptr<SystemSettings> sys) {
  kl::FilePath config_file{sys->source_folder + "/blog.config"};
  if (sys->verbose()) {
    kl::log("Blog processing started: {} => {}", sys->source_folder, config_file.fullPath());
  }
  settings.parse(config_file.fullPath());
  if (sys->verbose()) {
    settings.log_abstract();
  }
}

void Blog::process() const {}

} // namespace klblog
