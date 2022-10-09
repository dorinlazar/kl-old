#include "modulecollection.h"
#include "klbsettings.h"
#include "klbexecutor.h"
#include "klbsettings.h"

using namespace kl;

kl::uptr<ModuleCollection> discoverModules(kl::ptr<FSCache> cache) {
  auto mc = std::make_unique<ModuleCollection>(cache);
  mc->discoverModules();
  return mc;
}

int main(int argc, char** argv, char** envp) {
  CMD.Init(argc, argv, envp);
  auto fscache = std::make_shared<FSCache>(CMD.sourceFolder, CMD.buildFolder);

  auto mc = discoverModules(fscache);

  kl::Set<kl::Text> targets;

  for (const auto& target: CMD.targets) {
    kl::FilePath fp(target);
    // TODO implement a FilePath::startsWith that does this per path component
    if (target.startsWith(CMD.buildFolder.fullPath())) {
      fp = fp.remove_base_folder(CMD.buildFolder.folderDepth());
    } else if (target.startsWith(CMD.sourceFolder.fullPath())) {
      fp = fp.remove_base_folder(CMD.sourceFolder.folderDepth());
    }

    targets.add(mc->getModuleNames(fp.fullPath()));
  }

  auto modules = mc->getTargetModules(targets.toList());
  DefaultBuildStrategy sched(mc.get());

  for (const auto& mod: modules) {
    sched.build(mod);
  }

  for (const auto& mod: modules) {
    if (mod->hasMain()) {
      sched.link(mod);
    }
  }

  if (CMD.runMode) {
    for (const auto& mod: modules) {
      if (mod->hasMain()) {
        sched.run(mod);
      }
    }
  }

  return sched.execute() ? 0 : 1;
}
