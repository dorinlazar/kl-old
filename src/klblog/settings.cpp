#include "settings.h"

using namespace klblog;
using namespace kl;

Text Settings::version = "0.0.1";
Text Settings::targetFolder = ""_t;
bool Settings::verbose = false;
Dict<Text, Text> Settings::environment;
List<Text> Settings::arguments;

void Settings::parseCommandLine(int argc, char** argv, char** envp) {
  CHECK(argc > 0, "internal error: invalid number of arguments:", argc);
  for (int i = 1; i < argc; i++) {
    if ("-v"_t == argv[i]) {
      verbose = true;
    } else {
      arguments.add(argv[i]);
    }
  }
  while (*envp != nullptr) {
    auto [var, value] = kl::Text(*envp).splitNextChar('=');
    environment.add(var, value);
    envp++;
  }

  try {
    Queue<Text> args;
    args.push(arguments);
    while (!args.empty()) {
      auto arg = args.pop();
      if (arg == "-d") {
        targetFolder = args.pop();
      }
    }
  } catch (...) {
    FATAL("usage:", argv[0], "[-d <start_folder>]");
  }
}
