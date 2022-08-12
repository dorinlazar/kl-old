#pragma once

#include "module.h"

struct ModuleCollection {
  kl::ptr<Module> getModule(const kl::Text& filename) const;
  kl::ptr<Module> getModule(const kl::FilePath& folder, const kl::Text& filename) const;
  kl::ptr<Module> getOrCreateModule(const kl::FilePath& folder, const kl::Text& filename);

  kl::List<Module*> getTargetModules(const kl::List<kl::Text>& targets);
  kl::List<kl::Text> getModuleNames(const kl::Text& basePath);

  kl::FilePath resolvePath(const kl::Text& name, Module* origin) const;

  ModuleCollection(kl::ptr<FSCache> cache);

  void discoverModules();

  kl::Dict<kl::Text, kl::ptr<Module>> modules;

private:
  kl::ptr<FSCache> _cache;
  void _scanAllModules();
};
