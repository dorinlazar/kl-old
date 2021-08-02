#include "klbexecutor.h"
#include "klbtoolchain.h"
#include "klbsettings.h"
#include <thread>

enum class ExecStepType { Build, Link };

struct ExecStep {
  ExecStepType type;
  Module* module;
};

struct ExecutionStrategyImpl {
  kl::List<ExecStep> buildSteps;
  std::unique_ptr<Toolchain> toolchain;
  ModuleCollection* _coll;

  bool _performBuild(Module* mod) {
    auto src = mod->getSource();
    if (src.has_value() && mod->requiresBuild()) {
      if (!toolchain->build(src->path.fullPath(), mod->getObjectPath(), mod->includeFolders.toList())) {
        return false;
      }
      mod->updateObjectTimestamp(kl::DateTime::now());
    } else {
      if (CMD.verbose) {
        kl::log("Module:", mod->name, "doesn't require rebuild. Skipping...");
      }
    }
    return true;
  }

  bool _performLink(Module* mod) {
    if (mod->hasMain && mod->requiresLink()) {
      kl::Set<kl::Text> objects;
      objects.add(mod->getObject()->path.fullPath());
      for (const auto& m: mod->requiredModules) {
        auto depmod = _coll->getModule(m);
        CHECK(depmod != nullptr);
        auto object = depmod->getObject();
        if (object.has_value()) {
          objects.add(object->path.fullPath());
        }
      }
      if (!toolchain->link(objects.toList(), mod->getExecutablePath(), {})) {
        return false;
      }
    } else {
      if (CMD.verbose) {
        kl::log("Module:", mod->name, "doesn't require relink. Skipping...");
      }
    }
    return true;
  }

public:
  ExecutionStrategyImpl(ModuleCollection* coll) : _coll(coll) { toolchain = std::make_unique<Gcc>(); }
  void add(ExecStepType t, Module* mod) { buildSteps.add({.type = t, .module = mod}); }
  bool execute() { // the most basic strategy possible;
    for (const auto& step: buildSteps) {
      bool res = false;
      if (step.type == ExecStepType::Build) {
        res = _performBuild(step.module);
      } else {
        res = _performLink(step.module);
      }
      if (!res) {
        return false;
      }
    }
    return true;
  }
};

ExecutionStrategy::ExecutionStrategy(ModuleCollection* coll) : _modules(coll) {
  impl = std::make_unique<ExecutionStrategyImpl>(coll);
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

bool ExecutionStrategy::execute() {
  kl::log("Found", std::thread::hardware_concurrency(), "processors");
  return impl->execute();
}
void ExecutionStrategy::createBuildFolders() {
  kl::Set<kl::Text> directories;
  for (const auto& step: impl->buildSteps) {
    directories.add(kl::FilePath(CMD.buildFolder + "/"_t + step.module->name).folderName());
  }
  auto list = directories.toList().sortInPlace();

  for (const auto& dir: list) {
    bool made = mkDir(dir);
    if (CMD.verbose && made) {
      kl::log("Created folder", dir);
    }
  }
}
