#pragma once
#include "kl/kltext.h"
#include "kl/klfs.h"
#include "kl/klvalue.h"
#include "systemflags.h"

struct CommandParameters {
  bool verbose = false;
  kl::FilePath buildFolder;
  kl::FilePath sourceFolder;
  int32_t processorCount = 0;
  std::optional<int32_t> nJobs;
  bool runMode = false;
  kl::List<kl::Text> cxxFlags;
  kl::List<kl::Text> cFlags;
  kl::List<kl::Text> linkFlags;
  kl::uptr<SystemFlags> sysFlags;

  kl::List<kl::Text> arguments;
  kl::Dict<kl::Text, kl::Text> environment;
  kl::PValue configurationFile;

  kl::List<kl::Text> targets;

  void init(int argc, char** argv, char** envp);

private:
  void _updateFlags();
  void _updateSysEnv(char** envp);
  void _readDepotFile();
  void _processArguments(int argc, char** argv);
};

extern CommandParameters CMD;
#define VERSION_STRING "0.2-alpha"
