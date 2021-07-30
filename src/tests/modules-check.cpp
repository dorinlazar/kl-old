#include "klb/modules.h"
#include "klb/klbsettings.h"
#include "klb/depprocessor.h"
#include "testdep1.h"

using namespace kl;

int main() {
  FSCache fscache;
  fscache.addFolder(CMD.sourceFolder);
  fscache.addFolder(CMD.buildFolder);
  ModuleCollection mc(fscache);
  for (const auto& [path, folder]: fscache.all) {
    kl::log(folder);
  }

  for (const auto& [name, mod]: mc.modules) {
    mod->updateModuleInfo();
    // kl::log("Module:", name,
    //         mc.modules[name]->components.transform<kl::Text>([](const ModuleItem& mi) { return mi.extension; }),
    //         mod->hasMain ? "executable" : "non-executable", "HREQUIRE:", mod->resolvedLocalHeaderDeps);
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

  return 0;
}
