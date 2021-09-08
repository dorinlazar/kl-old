#include "modulecollection.h"
#include "klbsettings.h"
#include "klbexecutor.h"
#include "klbsettings.h"

using namespace kl;

kl::uptr<ModuleCollection> discoverModules(FSCache* cache) {
  auto mc = std::make_unique<ModuleCollection>(cache);
  mc->discoverModules();
  return mc;
}

int main(int argc, char** argv, char** envp) {
  CMD.init(argc, argv, envp);
  auto fscache = std::make_unique<FSCache>(CMD.sourceFolder, CMD.buildFolder);

  auto mc = discoverModules(fscache.get());

  kl::Set<kl::Text> targets;
  bool testMode = false;
  for (const auto& target: CMD.targets) {
    if (target == "test"_t) {
      testMode = true;
    } else {
      kl::FilePath fp(target);
      if (target.startsWith(CMD.buildFolder.fullPath())) {
        fp = fp.remove_base_folder(CMD.buildFolder.folderDepth());
      } else if (target.startsWith(CMD.sourceFolder.fullPath())) {
        fp = fp.remove_base_folder(CMD.sourceFolder.folderDepth());
      }

      targets.add(fp.fullPath());
    }
  }

  kl::List<Module*> tests;

  if (testMode) {
    targets.remove("test"_t);
    tests = mc->getExecutables("tests");
    kl::log("Tests:", tests.transform<kl::Text>([](Module* m) { return m->name(); }));
    targets.add(tests.transform<kl::Text>([](Module* m) { return m->name(); }));
  }

  auto modules = mc->getTargetModules(targets.toList());
  ExecutionStrategy sched(mc.get());

  for (const auto& mod: modules) {
    sched.build(mod);
  }

  for (const auto& mod: modules) {
    if (mod->hasMain()) {
      sched.link(mod);
    }
  }

  if (tests.size()) {
    for (const auto& mod: tests) {
      sched.run(mod);
    }
  }

  return sched.execute() ? 0 : 1;
}
