#include "klbsettings.h"
#include "kl/klfs.h"
#include <thread>
#include "ff/poorconfig.h"

void CommandParameters::_updateFlags() {
  auto cxxfl = configurationFile->getOpt("CXXFLAGS"_t);
  auto cfl = configurationFile->getOpt("CFLAGS"_t);
  auto link = configurationFile->getOpt("LDFLAGS"_t);

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
  if (environment.getOpt("VERBOSE").has_value() || configurationFile->getOpt("verbose").has_value()) {
    verbose = true;
  }

  auto tmp = environment.getOpt("JOBS");
  if (tmp.has_value()) {
    nJobs = tmp->toInt();
  } else {
    tmp = configurationFile->getOpt("build/jobs");
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
    auto [var, value] = txt.splitNextChar('=');
    environment.add(var, value);
    envp++;
  }

  processorCount = std::thread::hardware_concurrency();
  if (processorCount < 1) {
    processorCount = 1;
  }
}

void CommandParameters::_readDepotFile() {
  try {
    auto cfg = kl::FileReader(".depot.conf").readAll();

    configurationFile = kl::PoorConfig::parse(cfg);
  } catch (...) {
    if (verbose) {
      kl::log("No valid .depot.conf exists. Using defaults.");
    }
    configurationFile = kl::Value::createNull();
  }
}

void CommandParameters::_processArguments(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    arguments.add(argv[i]);
  }
  for (const auto& cmd: arguments) {
    if (cmd == "-v"_t) {
      verbose = true;
    } else if (cmd == "-r"_t) {
      runMode = true;
    } else {
      targets.add(cmd);
    }
  }
}

void CommandParameters::init(int argc, char** argv, char** envp) {
  buildFolder = kl::FilePath("build"_t);
  sourceFolder = kl::FilePath("src"_t);
  _processArguments(argc, argv);

  if (verbose) {
    kl::log("klb v" VERSION_STRING " - KeyLocked Build tool ©2021 Dorin Lazăr");
  }

  _readDepotFile();
  _updateSysEnv(envp);
  _updateFlags();
}

CommandParameters CMD;
