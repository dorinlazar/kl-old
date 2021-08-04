#include "klbsettings.h"
#include "kl/klfs.h"
#include <thread>

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
    cFlags = cfl->splitByChar(' ');
  }
  if (link.has_value()) {
    linkFlags = link->splitByChar(' ');
  }
  if (environment.getOpt("VERBOSE").has_value() || configurationFile.getOpt("VERBOSE").has_value()) {
    verbose = true;
  }

  auto tmp = environment.getOpt("JOBS");
  if (tmp.has_value()) {
    nJobs = tmp->toInt();
  } else {
    tmp = configurationFile.getOpt("JOBS");
    if (tmp.has_value()) {
      nJobs = tmp->toInt();
    }
  }
  if (nJobs.has_value() && (*nJobs > processorCount * 2)) {
    nJobs = processorCount * 2;
  }
}

void CommandParameters::_updateSysEnv(char** envp) {
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

  processorCount = std::thread::hardware_concurrency();
  if (processorCount < 1) {
    processorCount = 1;
  }
}

void CommandParameters::_readDepotFile() {
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
  }
}

void CommandParameters::_processArguments(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    arguments.add(argv[i]);
  }
  for (const auto& cmd: arguments) {
    if (cmd == "-v"_t) {
      verbose = true;
    }
  }
}

void CommandParameters::init(int argc, char** argv, char** envp) {
  _processArguments(argc, argv);
  if (verbose) {
    kl::log("klb v" VERSION_STRING " - KeyLocked Build tool ©2021 Dorin Lazăr");
  }

  _readDepotFile();
  _updateSysEnv(envp);
  _updateFlags();
}

CommandParameters CMD;
