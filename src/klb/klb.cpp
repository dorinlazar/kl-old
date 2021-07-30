#include "modules.h"
#include "klbsettings.h"
#include "depprocessor.h"
#include "klbexecutor.h"
#include "klbsettings.h"

using namespace kl;

int main(int argc, char** argv) {
  if (argc == 2 && kl::Text(argv[1]) == "-v"_t) {
    CMD.verbose = true;
  }

  if (CMD.verbose) {
    kl::log("klb v" VERSION_STRING " - KeyLocked Build tool ©2021 Dorin Lazăr");
  }

  FSCache fscache;
  fscache.addFolder(CMD.sourceFolder);
  fscache.addFolder(CMD.buildFolder);
  ModuleCollection mc(fscache);
  for (const auto& [path, folder]: fscache.all) {
    kl::log(folder);
  }

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

      kl::log("Executable", name, "REQUIRE:", proc.result());
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
