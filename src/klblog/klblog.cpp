#include "settings.h"
#include "ff/poorconfig.h"

using namespace klblog;

int main(int argc, char** argv, char** envp) {
  Settings::parseCommandLine(argc, argv, envp);
  if (Settings::verbose) {
    kl::log(("KLBlog v"_t + Settings::version + "© 2021 Dorin Lazăr, released under GPL v.2.1").toText().toView());
  }

  return 0;
}
