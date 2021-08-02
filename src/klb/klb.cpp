#include "modules.h"
#include "klbsettings.h"
#include "depprocessor.h"
#include "klbexecutor.h"
#include "klbsettings.h"

using namespace kl;

int main(int argc, char** argv, char** envp) {
  CMD.init(argc, argv, envp);
  FSCache fscache;
  fscache.addFolder(CMD.sourceFolder);
  fscache.addFolder(CMD.buildFolder);
  ModuleCollection mc(fscache);

  for (const auto& [name, mod]: mc.modules) {
    mod->updateModuleInfo();
  }

  Set<kl::Text> requiredModules;

  for (const auto& [name, mod]: mc.modules) {
    if (mod->hasMain) {
      DependencyProcessor<kl::Text> proc;
      proc.addItem(name);
      proc.process([&mc](const kl::Text& modName) {
        auto mod = mc.getModule(modName);
        CHECK(mod != nullptr, "Unable to find required module", modName);
        return mod->requiredModules.toList();
      });

      mod->requiredModules = proc.result();
      requiredModules.add(proc.result());
    }
  }

  ExecutionStrategy sched(&mc);

  for (const auto& modName: requiredModules) {
    sched.build(modName);
  }

  for (const auto& [name, mod]: mc.modules) {
    if (mod->hasMain) {
      sched.link(name);
    }
  }

  sched.createBuildFolders();

  sched.execute();

  return 0;
}
