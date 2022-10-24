#include "systemsettings.hpp"

namespace klblog {
using namespace kl;

SystemSettings::SystemSettings(int argc, char** argv, char** envp) {
  CHECK(argc > 0, "internal error: invalid number of arguments: {}", argc);
  for (int i = 1; i < argc; i++) {
    if ("-v"_t == argv[i]) {
      verbosity = VerbosityLevel::Verbose;
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
    kl::Queue<kl::Text> args;
    args.push(arguments);
    while (!args.empty()) {
      auto arg = args.pop();
      if (arg == "-d") {
        source_folder = args.pop();
      }
      if (arg == "-o") {
        destination_folder = args.pop();
      }
    }
  } catch (...) {
    FATAL("usage: {} [-d <source>] [-o <target>]", argv[0]);
  }
}

bool SystemSettings::verbose() const { return verbosity == VerbosityLevel::Verbose; }

} // namespace klblog
