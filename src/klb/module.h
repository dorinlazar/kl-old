#pragma once

#include "klbfs.h"
#include "moduleitem.h"

struct ModuleCollection;

struct Module {
  bool hasMain;
  kl::Set<kl::Text> headerLocalIncludes;
  kl::Set<kl::Text> headerSysIncludes;
  kl::Set<kl::Text> localIncludes;
  kl::Set<kl::Text> systemIncludes;
  kl::Set<kl::Text> requiredModules;
  kl::Set<kl::Text> resolvedLocalHeaderDeps;
  kl::Set<kl::Text> includeFolders;

public:
  Module(ModuleCollection* container, const kl::Text& seed);
  void addFile(const kl::FileInfo& fi);
  void scanModuleRequirements();
  void updateModuleInfo();

public:
  bool hasSource() const;
  kl::Text headerPath() const;
  kl::Text sourcePath() const;
  kl::Text objectPath() const;
  kl::Text executablePath() const;
  kl::Text buildFolder() const;
  const kl::Text& name() const;

  bool requiresBuild() const;
  bool requiresLink() const;
  void updateObjectTimestamp(kl::DateTime timestamp);

private:
  void _updateComponentPaths();
  void _scanSource();
  void _scanHeader();
  void _updateModuleDependencies();
  void _updateHeaderDependencies();
  void _updateIncludeFolders();
  kl::Text _resolveHeader(const kl::Text& name);
  std::optional<ModuleItem> getHeader() const { return header; }
  std::optional<ModuleItem> getSource() const { return source; }
  std::optional<ModuleItem> getObject() const { return object; }
  std::optional<ModuleItem> getExecutable() const { return executable; }

  kl::Text _name;
  ModuleCollection* _parent;

  kl::FilePath _buildPath;
  kl::FilePath _sourcePath;

  std::optional<ModuleItem> header;
  std::optional<ModuleItem> source;
  std::optional<ModuleItem> object;
  std::optional<ModuleItem> executable;
};
