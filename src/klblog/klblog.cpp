#include "systemsettings.hpp"
#include "ff/poorconfig.h"

int main(int argc, char** argv, char** envp) {
  klblog::SystemSettings system_settings(argc, argv, envp);
  if (system_settings.verbosity == klblog::VerbosityLevel::Verbose) {
    kl::log("KLBlog v{} © 2022 Dorin Lazăr, released under GPL v2.1", system_settings.version);
  }

  return 0;
}
