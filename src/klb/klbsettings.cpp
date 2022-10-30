#include "klbsettings.h"
#include "kl/klfs.h"
#include <thread>
#include <filesystem>
#include "ff/poorconfig.h"
using namespace kl::literals;

void CommandParameters::UpdateFlags() {
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
    m_cxx_flags = cxxfl->splitByChar(' ');
  }
  if (cfl.has_value()) {
    m_c_flags = cfl->splitByChar(' ');
  }
  if (link.has_value()) {
    m_link_flags = link->splitByChar(' ');
  }
  if (environment.getOpt("VERBOSE").has_value() || configurationFile->getOpt("verbose").has_value()) {
    m_verbose = true;
  }

  auto tmp = environment.getOpt("JOBS");
  m_n_jobs = processorCount * 2;
  if (tmp.has_value()) {
    m_n_jobs = tmp->toInt();
  } else {
    tmp = configurationFile->getOpt("build/jobs");
    if (tmp.has_value()) {
      m_n_jobs = tmp->toInt();
    }
  }

  kl::Value* sysflagsettings = nullptr;
  if (configurationFile->has("system"_t)) {
    sysflagsettings = configurationFile->get("system"_t).get();
  }

  m_sys_flags = std::make_unique<SystemFlags>(sysflagsettings);
}

void CommandParameters::UpdateSysEnv(char** envp) {
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
  m_current_working_directory = kl::Text(std::filesystem::current_path().c_str());
}

void CommandParameters::ReadDepotFile() {
  try {
    auto cfg = kl::FileReader(".depot.conf").readAll();

    configurationFile = kl::PoorConfig::parse(cfg);
  } catch (...) {
    if (m_verbose) {
      kl::log("No valid .depot.conf exists. Using defaults.");
    }
    configurationFile = kl::Value::createNull();
  }
}

void CommandParameters::ProcessArguments(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    arguments.add(argv[i]);
  }
  for (const auto& cmd: arguments) {
    if (cmd == "-v"_t) {
      m_verbose = true;
    } else if (cmd == "-r"_t) {
      m_run_mode = true;
    } else {
      m_targets.add(cmd);
    }
  }
}

void CommandParameters::Init(int argc, char** argv, char** envp) {
  m_build_folder = kl::FilePath("build"_t);
  m_source_folder = kl::FilePath("src"_t);
  ProcessArguments(argc, argv);

  if (m_verbose) {
    kl::log("klb v{} - KeyLocked Build tool ©2021 Dorin Lazăr", VERSION_STRING);
  }

  ReadDepotFile();
  UpdateSysEnv(envp);
  UpdateFlags();
}

bool CommandParameters::Verbose() const { return m_verbose; }
kl::FilePath CommandParameters::CurrentWorkingDirectory() const { return m_current_working_directory; }
kl::FilePath CommandParameters::BuildFolder() const { return m_build_folder; }
kl::FilePath CommandParameters::SourceFolder() const { return m_source_folder; }
const kl::List<kl::Text>& CommandParameters::Targets() const { return m_targets; }
const kl::List<kl::Text>& CommandParameters::CFlags() const { return m_c_flags; }
const kl::List<kl::Text>& CommandParameters::CxxFlags() const { return m_cxx_flags; }
const kl::List<kl::Text>& CommandParameters::LinkFlags() const { return m_link_flags; }
bool CommandParameters::DoRun() const { return m_run_mode; }
const SystemFlags& CommandParameters::SysFlags() const { return *m_sys_flags; }
uint32_t CommandParameters::JobsCount() const { return m_n_jobs; }

CommandParameters CMD;
