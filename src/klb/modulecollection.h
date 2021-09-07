#pragma once

#include "module.h"

struct ModuleCollection {
  kl::Dict<kl::Text, kl::ptr<Module>> modules;
  kl::ptr<Module> getModule(const kl::Text& filename) const;
  kl::ptr<Module> getModule(const kl::FilePath& folder, const kl::Text& filename) const;
  kl::ptr<Module> getOrCreateModule(const kl::FilePath& folder, const kl::Text& filename);

  ModuleCollection(FSCache* cache) : _cache(cache) {}

  void discoverAll();
  void discoverTests();

private:
  FSCache* _cache;
  void _scanAllModules();
  void _scanModules(const kl::List<kl::Text>& targets);
};
