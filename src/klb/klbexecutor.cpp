#include "klbexecutor.h"
#include "klbtoolchain.h"
#include "klbsettings.h"
#include "kl/klprocess.h"
#include <thread>

enum class ExecStepType { Build, Link };

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
        auto cmdLine = _toolchain->buildCmdLine(mod->sourcePath(), mod->objectPath(), mod->includeFolders());
        auto node = _horde.addNode(cmdLine, {});
        _execNodes.add(mod->objectPath(), node);
        mod->updateObjectTimestamp(kl::DateTime::MAX);
      }
    } else if (t == ExecStepType::Link) {
      if (mod->requiresLink()) {
        auto objects = getDependentObjects(mod);
        auto depNodes = objects.transform<kl::ExecutionNode*>([this](const kl::Text& o) { return _execNodes.get(o); })
                            .select([](const kl::ExecutionNode* t) { return t != nullptr; });
        auto cmdLine = _toolchain->linkCmdLine(objects, mod->executablePath(), CMD.linkFlags);
        auto node = _horde.addNode(cmdLine, depNodes);
        _execNodes.add(mod->executablePath(), node);
      } else if (CMD.verbose) {
        kl::log("Module", mod->name(), "requires no linking");
      }
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

ExecutionStrategy::ExecutionStrategy(ModuleCollection* coll) : _modules(coll) {
  impl = std::make_unique<ParallelExecutionStrategy>(coll);
}

ExecutionStrategy::~ExecutionStrategy() {}
void ExecutionStrategy::build(const kl::Text& module) {
  auto mod = _modules->getModule(module);
  CHECK(mod != nullptr, "Module not found:", module);
  impl->add(ExecStepType::Build, mod.get());
}

void ExecutionStrategy::link(const kl::Text& module) {
  auto mod = _modules->getModule(module);
  CHECK(mod != nullptr, "Module not found:", module);
  impl->add(ExecStepType::Link, mod.get());
}

bool ExecutionStrategy::execute() { return impl->execute(); }
