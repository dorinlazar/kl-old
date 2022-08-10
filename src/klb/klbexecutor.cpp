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
  kl::uptr<Toolchain> _toolchain;
  ModuleCollection* _modules;

public:
  ExecutionStrategyImpl(ModuleCollection* coll) : _modules(coll) { _toolchain = std::make_unique<Gcc>(); }

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

class ParallelExecutionStrategy : public ExecutionStrategyImpl {
  kl::ProcessHorde _horde;
  kl::Set<kl::Text> _buildFolders;
  kl::Dict<kl::Text, kl::ExecutionNode*> _execNodes;

public:
  ParallelExecutionStrategy(ModuleCollection* coll) : ExecutionStrategyImpl(coll) {}

  void add(ExecStepType t, Module* mod) override {
    if (t == ExecStepType::Build) {
      if (mod->requiresBuild()) {
        _buildFolders.add(mod->buildFolder());
        auto cmdLine = _toolchain->buildCmdLine(mod->sourcePath(), mod->objectPath(), mod->includeFolders(),
                                                CMD.sysFlags->cxxflags(mod->sourceSystemHeaders()));
        auto node = _horde.addNode(cmdLine, {});
        _execNodes.add(mod->objectPath(), node);
        mod->updateObjectTimestamp(kl::DateTime::MAX);
      }
    } else if (t == ExecStepType::Link) {
      if (mod->requiresLink()) {
        auto objects = getDependentObjects(mod);
        auto depNodes = objects.transform<kl::ExecutionNode*>([this](const kl::Text& o) { return _execNodes.get(o); })
                            .select([](const kl::ExecutionNode* t) { return t != nullptr; });
        auto cmdLine = _toolchain->linkCmdLine(objects, mod->executablePath(),
                                               CMD.sysFlags->ldflags(mod->recursiveSystemHeaders()));
        auto node = _horde.addNode(cmdLine, depNodes);
        _execNodes.add(mod->executablePath(), node);
      } else if (CMD.verbose) {
        kl::log("Module {} requires no linking");
      }
    } else if (t == ExecStepType::Run) {
      auto exe = mod->executablePath();
      kl::List<kl::ExecutionNode*> nodes;
      if (mod->requiresLink()) {
        nodes.add(_execNodes[exe]);
      }
      auto node = _horde.addNode({exe}, nodes);
      _execNodes.add("RUN_"_t + mod->executablePath(), node);
    }
  }

  bool execute() override {
    createBuildFolders();
    return _horde.run(CMD.nJobs.value_or(2), true);
  }

  void createBuildFolders() {
    auto list = _buildFolders.toList().sortInPlace();

    for (const auto& dir: list) {
      bool made = kl::FileSystem::makeDirectory(dir);
      if (CMD.verbose && made) {
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

GenMakefileStrategy::GenMakefileStrategy(ModuleCollection* coll, kl::Text filename)
    : BuildStrategy(coll), _output(filename.toString(), std::ios::trunc) {
  _output << ".PHONY: executables makedirs\n";
  _output << "all: makedirs executables\n";
}

GenMakefileStrategy::~GenMakefileStrategy() {
  _output << "executables: " << kl::TextChain(_build_targets).join(' ').toView() << "\n";
  _output << "makedirs:\n\tmkdir -p " << kl::TextChain(_build_dirs.toList()).join(' ').toView() << "\n";
}

void GenMakefileStrategy::build(Module* mod) {
  Gcc toolchain;
  _build_dirs.add(mod->buildFolder());
  auto deps = mod->requiredModules()
                  .select([](Module* pmod) { return pmod->hasHeader(); })
                  .transform<kl::Text>([](Module* ptr) { return ptr->headerPath(); });
  _output << mod->objectPath().toView() << ": " << mod->sourcePath().toView() << " "
          << kl::TextChain(deps).join(' ').toView() << "\n\t";
  auto cmdLine = toolchain.buildCmdLine(mod->sourcePath(), mod->objectPath(), mod->includeFolders(),
                                        CMD.sysFlags->cxxflags(mod->sourceSystemHeaders()));
  _output << kl::TextChain(cmdLine).join(' ').toView() << "\n";
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
      toolchain.linkCmdLine(objects, mod->executablePath(), CMD.sysFlags->ldflags(mod->recursiveSystemHeaders()));
  _output << mod->executablePath().toView() << ": " << kl::TextChain(objects).join(' ').toView() << "\n\t"
          << kl::TextChain(cmdLine).join(' ').toView() << "\n";
  _build_targets.add(mod->executablePath());
}

void GenMakefileStrategy::run(Module*) {}
bool GenMakefileStrategy::execute() { return true; }
