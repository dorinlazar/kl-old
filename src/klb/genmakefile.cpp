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
  auto fscache = std::make_shared<FSCache>(CMD.SourceFolder(), CMD.BuildFolder());

  auto mc = discoverModules(fscache);
  auto modules = mc->getTargetModules({});
  kl::Text target_makefile = "Makefile";
  kl::Text target_compilation_database = CMD.BuildFolder().full_path() + "/compile_commands.json";
  if (CMD.Targets().size() > 0) {
    target_makefile = CMD.Targets()[0];
  }
  GenMakefileStrategy sched(mc.get(), target_makefile, target_compilation_database);

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
