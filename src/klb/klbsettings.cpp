#include "klbsettings.h"
#include "kl/klfs.h"

CommandParameters CMD;

void CommandParameters::_updateFlags() {
  auto cxxfl = configurationFile.getOpt("CXXFLAGS"_t);
  auto cfl = configurationFile.getOpt("CFLAGS"_t);
  auto link = configurationFile.getOpt("LDFLAGS"_t);

  if (!cxxfl.has_value()) {
    cxxfl = environment.getOpt("CXXFLAGS"_t);
  }

  if (!cfl.has_value()) {
    cfl = environment.getOpt("CFLAGS"_t);
  }

  if (!link.has_value()) {
    link = environment.getOpt("CXXFLAGS"_t);
  }

  if (cxxfl.has_value()) {
    cxxFlags = cxxfl->splitByChar(' ');
  }
  if (cfl.has_value()) {
    CMD.cFlags = cfl->splitByChar(' ');
  }
  if (link.has_value()) {
    CMD.linkFlags = link->splitByChar(' ');
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
      environment.add(txt, ""_t);
    }
    envp++;
  }

  // simplified .depot.conf
  auto text = kl::readFile(".depot.conf");
  auto cfg = text.splitLines(kl::SplitEmpty::Discard);
  for (const auto& txt: cfg) {
    auto pos = txt.pos('=');
    if (pos.has_value()) {
      configurationFile.add(txt.sublen(0, *pos), txt.subpos((*pos) + 1, txt.size()));
    } else {
      configurationFile.add(txt, ""_t);
    }
    envp++;
  }

  _updateFlags();
}
