#include "klbexecutor.h"
#include "klbtoolchain.h"
#include "klbsettings.h"
#include "kl/klprocess.h"
#include <thread>
using namespace kl::literals;

enum class ExecStepType { Build, Link, Run };

struct ExecStep {
  ExecStepType type;
  Module* module;
};

class ExecutionStrategyImpl {
protected:
  kl::uptr<Toolchain> m_toolchain;
  ModuleCollection* m_modules;

public:
  ExecutionStrategyImpl(ModuleCollection* coll) : m_modules(coll) { m_toolchain = std::make_unique<Gcc>(); }
  virtual ~ExecutionStrategyImpl() = default;

  virtual void add(ExecStepType t, Module* mod) = 0;
  virtual bool execute() = 0;

  kl::List<kl::Text> getDependentObjects(Module* mod) {
    kl::Set<kl::Text> objects;
    objects.add(mod->objectPath());
    for (const auto& m: mod->requiredModules()) {
      if (m->hasSource()) {
        objects.add(m->objectPath());
      }
    }
    return objects.toList();
  }
};

class ParallelExecutionStrategy final : public ExecutionStrategyImpl {
  kl::ProcessHorde m_horde;
  kl::Set<kl::Text> m_build_folders;
  kl::Dict<kl::Text, kl::ExecutionNode*> m_exec_nodes;

public:
  ParallelExecutionStrategy(ModuleCollection* coll) : ExecutionStrategyImpl(coll) {}
  ~ParallelExecutionStrategy() override = default;
  void add(ExecStepType t, Module* mod) override {
    if (t == ExecStepType::Build) {
      if (mod->requiresBuild()) {
        m_build_folders.add(mod->buildFolder());
        auto cmdLine = m_toolchain->buildCmdLine(mod->sourcePath(), mod->objectPath(), mod->includeFolders(),
                                                 CMD.SysFlags().cxxflags(mod->sourceSystemHeaders()));
        auto node = m_horde.add_node(cmdLine, {});
        m_exec_nodes.add(mod->objectPath(), node);
        mod->updateObjectTimestamp(kl::DateTime::MAX);
      }
    } else if (t == ExecStepType::Link) {
      if (mod->requiresLink()) {
        auto objects = getDependentObjects(mod);
        auto depNodes = objects.transform<kl::ExecutionNode*>([this](const kl::Text& o) { return m_exec_nodes.get(o); })
                            .select([](const kl::ExecutionNode* t) { return t != nullptr; });
        auto cmdLine = m_toolchain->linkCmdLine(objects, mod->executablePath(),
                                                CMD.SysFlags().ldflags(mod->recursiveSystemHeaders()));
        auto node = m_horde.add_node(cmdLine, depNodes);
        m_exec_nodes.add(mod->executablePath(), node);
      } else if (CMD.Verbose()) {
        kl::log("Module {} requires no linking");
      }
    } else if (t == ExecStepType::Run) {
      auto exe = mod->executablePath();
      kl::List<kl::ExecutionNode*> nodes;
      if (mod->requiresLink()) {
        nodes.add(m_exec_nodes[exe]);
      }
      auto node = m_horde.add_node({exe}, nodes);
      m_exec_nodes.add("RUN_"_t + mod->executablePath(), node);
    }
  }

  bool execute() override {
    createBuildFolders();
    return m_horde.run(CMD.JobsCount(), true);
  }

  void createBuildFolders() {
    auto list = m_build_folders.toList().sortInPlace();

    for (const auto& dir: list) {
      bool made = kl::FileSystem::makeDirectory(dir);
      if (CMD.Verbose() && made) {
        kl::log("Created folder", dir);
      }
    }
  }
};

BuildStrategy::BuildStrategy(ModuleCollection* coll) : _modules(coll) {}
BuildStrategy::~BuildStrategy() {}

DefaultBuildStrategy::DefaultBuildStrategy(ModuleCollection* coll) : BuildStrategy(coll) {
  impl = std::make_unique<ParallelExecutionStrategy>(coll);
}

DefaultBuildStrategy::~DefaultBuildStrategy() {}
void DefaultBuildStrategy::build(Module* mod) { impl->add(ExecStepType::Build, mod); }
void DefaultBuildStrategy::link(Module* mod) { impl->add(ExecStepType::Link, mod); }
void DefaultBuildStrategy::run(Module* mod) { impl->add(ExecStepType::Run, mod); }
bool DefaultBuildStrategy::execute() { return impl->execute(); }

GenMakefileStrategy::GenMakefileStrategy(ModuleCollection* coll, kl::Text makefile_filename,
                                         kl::Text compilation_db_filename)
    : BuildStrategy(coll), m_makefile_output(makefile_filename.toString(), std::ios::trunc),
      m_compilation_db_output(compilation_db_filename.toString(), std::ios::trunc) {
  m_makefile_output << ".PHONY: executables makedirs\n";
  m_makefile_output << "all: makedirs executables\n";
  m_compilation_db_output << "[";
}

GenMakefileStrategy::~GenMakefileStrategy() {
  m_makefile_output << "executables: " << kl::TextChain(_build_targets).join(' ').toView() << "\n";
  m_makefile_output << "makedirs:\n\tmkdir -p " << kl::TextChain(_build_dirs.toList()).join(' ').toView() << "\n";
  m_compilation_db_output << "\n]\n";
}

void GenMakefileStrategy::build(Module* mod) {
  Gcc toolchain;
  _build_dirs.add(mod->buildFolder());
  auto deps = mod->requiredModules()
                  .select([](Module* pmod) { return pmod->hasHeader(); })
                  .transform<kl::Text>([](Module* ptr) { return ptr->headerPath(); });
  m_makefile_output << mod->objectPath().toView() << ": " << mod->sourcePath().toView() << " "
                    << kl::TextChain(deps).join(' ').toView() << "\n\t";

  auto cmdLine = toolchain.buildCmdLine(mod->sourcePath(), mod->objectPath(), mod->includeFolders(),
                                        CMD.SysFlags().cxxflags(mod->sourceSystemHeaders()));
  m_makefile_output << kl::TextChain(cmdLine).join(' ').toView() << "\n\n";
  if (m_first_operation) {
    m_first_operation = false;
  } else {
    m_compilation_db_output << ",";
  }
  m_compilation_db_output << "\n{\n";
  m_compilation_db_output << " \"directory\": " << CMD.CurrentWorkingDirectory().fullPath().quote_escaped() << ",\n";
  m_compilation_db_output << " \"file\": " << mod->sourcePath().quote_escaped() << ",\n";
  cmdLine.add("-isystem"_t);
  cmdLine.add("/usr/include"_t);
  m_compilation_db_output << " \"arguments\": [" << kl::TextChain(cmdLine.transform<kl::Text>([](const kl::Text& t) {
                                                      return t.quote_escaped();
                                                    })).join(", ")
                          << "]\n";
  m_compilation_db_output << "}";
}

kl::List<kl::Text> getDepObjects(Module* mod) {
  kl::Set<kl::Text> objects;
  objects.add(mod->objectPath());
  for (const auto& m: mod->requiredModules()) {
    if (m->hasSource()) {
      objects.add(m->objectPath());
    }
  }
  return objects.toList();
}

void GenMakefileStrategy::link(Module* mod) {
  Gcc toolchain;
  auto objects = getDepObjects(mod);
  auto cmdLine =
      toolchain.linkCmdLine(objects, mod->executablePath(), CMD.SysFlags().ldflags(mod->recursiveSystemHeaders()));
  m_makefile_output << mod->executablePath().toView() << ": " << kl::TextChain(objects).join(' ').toView() << "\n\t"
                    << kl::TextChain(cmdLine).join(' ').toView() << "\n\n";
  _build_targets.add(mod->executablePath());
}

void GenMakefileStrategy::run(Module*) {}
bool GenMakefileStrategy::execute() { return true; }
