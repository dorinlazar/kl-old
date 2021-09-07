#include "modulecollection.h"
#include "klbsettings.h"
#include "depprocessor.h"
#include "klbexecutor.h"
#include "klbsettings.h"

using namespace kl;

kl::uptr<ModuleCollection> discoverModules(const kl::List<kl::Text>& targets, FSCache* cache) {
  auto mc = std::make_unique<ModuleCollection>(cache);
  if (targets.size() == 0) {
    mc->discoverAll();
  } else {
    kl::log("Target-based builds are work in progress!!!");
    if (targets.has("test")) { // let's try what we really need first.
      mc->discoverTests();
    }
  }

  return mc;
}

int main(int argc, char** argv, char** envp) {
  CMD.init(argc, argv, envp);
  auto fscache = std::make_unique<FSCache>(CMD.sourceFolder, CMD.buildFolder);

  auto mc = discoverModules(CMD.targets, fscache.get());

  Set<kl::Text> requiredModules;

  for (const auto& [name, mod]: mc->modules) {
    if (mod->hasMain()) {
      requiredModules.add(mod->requiredModules().transform<kl::Text>([](Module* m) { return m->name(); }));
    }
  }

  ExecutionStrategy sched(mc.get());

  for (const auto& modName: requiredModules) {
    sched.build(modName);
  }

  for (const auto& [name, mod]: mc->modules) {
    if (mod->hasMain()) {
      sched.link(name);
    }
  }

  return sched.execute() ? 0 : 1;
}
