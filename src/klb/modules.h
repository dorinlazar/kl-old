#pragma once

#include "klbfs.h"

enum class ModuleItemType { Unknown, Header, Code, Object, Executable };

struct ModuleItem {
  kl::Text extension;
  kl::DateTime timestamp;
  ModuleItemType type;
  kl::FilePath relPath;
  kl::FilePath path;

  void _updateModuleType();

public:
  ModuleItem(const kl::FileInfo& fi);
  ModuleItem(const kl::FilePath& fullPath, kl::DateTime timestamp);
};

struct ModuleCollection;

struct Module {
  kl::Text name;
  kl::List<ModuleItem> components;
  bool hasMain;
  kl::Set<kl::Text> headerLocalIncludes;
  kl::Set<kl::Text> headerSysIncludes;
  kl::Set<kl::Text> localIncludes;
  kl::Set<kl::Text> systemIncludes;
  ModuleCollection* parent;
  kl::Set<kl::Text> requiredModules;
  kl::Set<kl::Text> resolvedLocalHeaderDeps;
  kl::Set<kl::Text> includeFolders;
  kl::DateTime headerTimeStamp;

public:
  Module(ModuleCollection* container, const kl::Text& seed);
  void addFile(const kl::FileInfo& fi);
  void updateModuleInfo();
  std::optional<ModuleItem> getHeader() const;
  std::optional<ModuleItem> getSource() const;
  std::optional<ModuleItem> getObject() const;
  std::optional<ModuleItem> getExecutable() const;
  kl::Text getObjectPath() const;
  kl::Text getExecutablePath() const;
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
};

struct ModuleCollection {
  kl::Dict<kl::Text, std::shared_ptr<Module>> modules;
  std::shared_ptr<Module> getModule(const kl::Text& filename) const;
  std::shared_ptr<Module> getModule(const kl::FilePath& folder, const kl::Text& filename) const;
  std::shared_ptr<Module> makeModule(const kl::FilePath& folder, const kl::Text& filename);

  ModuleCollection(const FSCache& cache);

private:
  void _updateModuleDependencies();
  void _scanModules(const FSCache& cache);
};
