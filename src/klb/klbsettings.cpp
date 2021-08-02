#include "klbsettings.h"

CommandParameters CMD;

void _updateFlags() {
  auto cxxflags = CMD.environment.getOpt("CXXFLAGS"_t);
  auto cflags = CMD.environment.getOpt("CFLAGS"_t);
  auto linkflags = CMD.environment.getOpt("LDFLAGS"_t);
  if (cxxflags.has_value()) {
    CMD.cxxFlags = cxxflags->splitByChar(' ');
  }
  if (cflags.has_value()) {
    CMD.cFlags = cflags->splitByChar(' ');
  }
  if (linkflags.has_value()) {
    CMD.linkFlags = linkflags->splitByChar(' ');
  }
}

void CommandParameters::init(int argc, char** argv, char** envp) {
  if (argc == 2 && kl::Text(argv[1]) == "-v"_t) {
    CMD.verbose = true;
  }

  if (CMD.verbose) {
    kl::log("klb v" VERSION_STRING " - KeyLocked Build tool ©2021 Dorin Lazăr");
  }

  while (*envp != nullptr) {
    kl::Text txt(*envp);
    auto pos = txt.pos('=');
    if (pos.has_value()) {
      environment.add(txt.sublen(0, *pos), txt.subpos((*pos) + 1, txt.size()));
    } else {
      environment[txt] = ""_t;
    }
    envp++;
  }

  _updateFlags();
}
