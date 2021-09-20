#pragma once

#include "klbfs.h"
#include "moduleitem.h"

struct ModuleCollection;

struct Module {
  Module(ModuleCollection* container, const kl::Text& seed);
  ~Module();
  void addFile(const kl::FileSystemEntryInfo& fi);
  void readDirectRequirements();
  void updateHeaderDependencies();
  void updateModuleDependencies();
  void recurseModuleDependencies();

public:
  bool hasSource() const;
  bool hasMain() const;
  kl::Text headerPath() const;
  kl::Text sourcePath() const;
  kl::Text objectPath() const;
  kl::Text executablePath() const;
  kl::Text buildFolder() const;
  const kl::Text& name() const;
  const kl::List<Module*>& requiredModules() const;
  kl::List<kl::Text> includeFolders() const;

  bool requiresBuild() const;
  bool requiresLink() const;
  void updateObjectTimestamp(kl::DateTime timestamp);

private:
  void _updateComponentPaths();
  void _scanSource();
  void _scanHeader();

  struct ModuleInternals;
  kl::uptr<ModuleInternals> d;
};
