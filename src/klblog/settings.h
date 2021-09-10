#pragma once

#include "kl/kltext.h"
#include "kl/klds.h"

namespace klblog {
using namespace kl;

struct Settings {

  static void parseCommandLine(int argc, char** argv, char** envp);

  static Dict<Text, Text> environment;
  static Text version;
  static bool verbose;
};

} // namespace klblog
