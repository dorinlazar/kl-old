#include "settings.h"

using namespace klblog;
using namespace kl;

Text Settings::version = "0.0.1";
bool Settings::verbose = false;
Dict<Text, Text> Settings::environment;

void Settings::parseCommandLine(int argc, char** argv, char** envp) {
  for (int i = 0; i < argc; i++) {
    if ("-v"_t == argv[i]) {
      verbose = true;
    }
  }
  while (*envp != nullptr) {
    auto [var, value] = kl::Text(*envp).splitNextChar('=');
    environment.add(var, value);
    envp++;
  }
}
