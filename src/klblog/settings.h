#pragma once

#include "kl/kltext.h"
#include "kl/klds.h"

namespace klblog {
using namespace kl;

struct Settings {

  static void parseCommandLine(int argc, char** argv, char** envp);

  static Dict<Text, Text> environment;
  static List<Text> arguments;
  static Text version;
  static bool verbose;
  static Text targetFolder;
};

} // namespace klblog
