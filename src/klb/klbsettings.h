#pragma once
#include "kl/kltext.h"
#include "kl/klfs.h"
#include "kl/klvalue.h"
#include "systemflags.h"

struct CommandParameters {
  void Init(int argc, char** argv, char** envp);
  bool Verbose() const;
  kl::FilePath BuildFolder() const;
  kl::FilePath SourceFolder() const;
  kl::FilePath CurrentWorkingDirectory() const;
  const kl::List<kl::Text>& Targets() const;
  const kl::List<kl::Text>& CFlags() const;
  const kl::List<kl::Text>& CxxFlags() const;
  const kl::List<kl::Text>& LinkFlags() const;
  bool DoRun() const;

  const SystemFlags& SysFlags() const;
  uint32_t JobsCount() const;

private:
  bool m_verbose = false;
  kl::FilePath m_build_folder;
  kl::FilePath m_source_folder;
  kl::FilePath m_current_working_directory;
  int32_t processorCount = 0;
  uint32_t m_n_jobs;
  bool m_run_mode = false;
  kl::List<kl::Text> m_cxx_flags;
  kl::List<kl::Text> m_c_flags;
  kl::List<kl::Text> m_link_flags;
  kl::uptr<SystemFlags> m_sys_flags;

  kl::List<kl::Text> arguments;
  kl::Dict<kl::Text, kl::Text> environment;
  kl::PValue configurationFile;

  kl::List<kl::Text> m_targets;

private:
  void UpdateFlags();
  void UpdateSysEnv(char** envp);
  void ReadDepotFile();
  void ProcessArguments(int argc, char** argv);
};

extern CommandParameters CMD;
constexpr std::string_view VERSION_STRING = "0.2-alpha";
