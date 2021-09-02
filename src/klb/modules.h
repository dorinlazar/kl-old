#pragma once

#include "klbfs.h"
#include "moduleitem.h"

struct ModuleCollection;

struct Module {
  kl::Text name;
  std::optional<ModuleItem> header;
  std::optional<ModuleItem> source;
  std::optional<ModuleItem> object;
  std::optional<ModuleItem> executable;
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

public:
  bool hasSource() const;
  kl::Text getHeaderPath() const;
  kl::Text getSourcePath() const;
  kl::Text getObjectPath() const;
  kl::Text getExecutablePath() const;

  kl::Text getBuildFolder() const;

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

  kl::FilePath _buildPath;
  kl::FilePath _sourcePath;
};

struct ModuleCollection {
  kl::Dict<kl::Text, std::shared_ptr<Module>> modules;
  std::shared_ptr<Module> getModule(const kl::Text& filename) const;
  std::shared_ptr<Module> getModule(const kl::FilePath& folder, const kl::Text& filename) const;
  std::shared_ptr<Module> getOrCreateModule(const kl::FilePath& folder, const kl::Text& filename);

  ModuleCollection(FSCache* cache) : _cache(cache) {}

  void discoverAll();
  void discoverTests();

private:
  FSCache* _cache;
  void _updateModuleDependencies();
  void _scanAllModules();
  void _scanModules(const kl::List<kl::Text>& targets);
};
