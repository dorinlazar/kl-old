#pragma once

#include "klbfs.h"
#include "moduleitem.h"

struct ModuleCollection;

struct Module {

public:
  Module(ModuleCollection* container, const kl::Text& seed);
  void addFile(const kl::FileInfo& fi);
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

  kl::Text _name;
  ModuleCollection* _parent;
  bool _hasMain;
  kl::List<kl::Text> _headerLocalIncludes;
  kl::List<kl::Text> _headerSysIncludes;
  kl::List<kl::Text> _localIncludes;
  kl::List<kl::Text> _systemIncludes;
  kl::List<Module*> _requiredModules;
  kl::List<kl::Text> _resolvedLocalHeaderDeps;

  kl::FilePath _buildPath;
  kl::FilePath _sourcePath;

  std::optional<ModuleItem> _header;
  std::optional<ModuleItem> _source;
  std::optional<ModuleItem> _object;
  std::optional<ModuleItem> _executable;
};
