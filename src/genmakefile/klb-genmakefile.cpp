#include "klb/modulecollection.h"
#include "klb/klbsettings.h"
#include "klb/klbexecutor.h"
#include "klb/klbsettings.h"

using namespace kl;

kl::uptr<ModuleCollection> discoverModules(kl::ptr<FSCache> cache) {
  auto mc = std::make_unique<ModuleCollection>(cache);
  mc->discoverModules();
  return mc;
}

int main(int argc, char** argv, char** envp) {
  CMD.Init(argc, argv, envp);
  auto fscache = std::make_shared<FSCache>(CMD.SourceFolder(), CMD.BuildFolder());

  auto mc = discoverModules(fscache);
  auto modules = mc->getTargetModules({});
  kl::Text target_makefile = "Makefile";
  if (CMD.Targets().size() > 0) {
    target_makefile = CMD.Targets()[0];
  }
  GenMakefileStrategy sched(mc.get(), target_makefile);

  for (const auto& mod: modules) {
    sched.build(mod);
  }

  for (const auto& mod: modules) {
    if (mod->hasMain()) {
      sched.link(mod);
    }
  }

  return sched.execute() ? 0 : 1;
}
