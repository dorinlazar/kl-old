#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"
using namespace kl::literals;

ModuleCollection::ModuleCollection(kl::ptr<FSCache> cache) : _cache(cache) {}

void ModuleCollection::discoverModules() {
  _scanAllModules();
  for (const auto& [name, mod]: modules) {
    mod->readDirectRequirements();
  }
  for (const auto& [name, mod]: modules) {
    mod->updateHeaderDependencies();
  }
  for (const auto& [name, mod]: modules) {
    mod->updateModuleDependencies();
  }
  for (const auto& [name, mod]: modules) {
    if (mod->hasMain()) {
      mod->recurseModuleDependencies();
    }
  }
}

void ModuleCollection::_scanAllModules() {
  uint32_t srcDepth = CMD.SourceFolder().folderDepth();
  uint32_t bldDepth = CMD.BuildFolder().folderDepth();
  for (auto folder: _cache->getAllSourceFolders()) {
    auto path = folder->fullPath();
    auto modFld = path == CMD.SourceFolder() ? ""_t : path.discardBaseFolder(srcDepth);
    for (const auto& file: folder->files()) {
      auto mod = getOrCreateModule(modFld, file.path.stem());
      mod->addFile(file);
    }
  }
  for (auto folder: _cache->getAllBuildFolders()) {
    auto path = folder->fullPath();
    auto moduleFolder = path == CMD.BuildFolder() ? ""_t : path.discardBaseFolder(bldDepth);
    for (const auto& file: folder->files()) {
      auto mod = getModule(moduleFolder.fullPath(), file.path.stem());
      if (mod) {
        mod->addFile(file);
      }
    }
  }
}

kl::ptr<Module> ModuleCollection::getModule(const kl::FilePath& folder, const kl::Text& file) const {
  kl::Text moduleName = folder.add(file).replace_extension(""_t).fullPath();
  auto mod = modules.get(moduleName);
  return mod;
}

kl::ptr<Module> ModuleCollection::getModule(const kl::Text& file) const {
  kl::Text moduleName = kl::FilePath(file).replace_extension(""_t).fullPath();
  auto mod = modules.get(moduleName);
  return mod;
}

kl::ptr<Module> ModuleCollection::getOrCreateModule(const kl::FilePath& folder, const kl::Text& stem) {
  kl::Text moduleName = folder.add(stem).fullPath();
  auto val = modules.get(moduleName, nullptr);
  if (!val) {
    val = std::make_shared<Module>(this, moduleName);
    modules.add(moduleName, val);
  }
  return val;
}

kl::FilePath ModuleCollection::resolvePath(const kl::Text& name, Module* origin) const {
  auto path = CMD.SourceFolder().add(name);
  if (!_cache->fileExists(path)) {
    path = kl::FilePath(CMD.SourceFolder().add(origin->name()).folderName()).add(name);
    CHECK(_cache->fileExists(path), "Unable to locate dependency {} included in module {}. Tried: {} {}", name,
          origin->name(), CMD.SourceFolder().add(name).fullPath(), path.fullPath());
  }
  return path.discardBaseFolder(CMD.SourceFolder().folderDepth());
}

kl::List<Module*> ModuleCollection::getTargetModules(const kl::List<kl::Text>& targets) {
  kl::Set<Module*> requiredModules;
  if (targets.size() == 0) {
    for (const auto& [name, mod]: modules) {
      if (mod->hasMain()) {
        requiredModules.add(mod->requiredModules());
      }
    }
  } else {
    for (const auto& file: targets) {
      kl::FilePath fp(file);
      kl::Text moduleName = fp.replace_extension(""_t).fullPath();
      auto mod = modules.get(moduleName);
      if (!mod) [[unlikely]] {
        FATAL("Unable to identify target {}", file);
      }
      auto ext = fp.extension();
      if (ext == "o"_t) {
        requiredModules.add(mod.get());
      } else if (mod->hasMain() && (ext == "exe"_t || ext == ""_t || ext.startsWith("c"))) {
        requiredModules.add(mod->requiredModules());
      } else {
        FATAL("Don't know how to build {}", file);
      }
    }
  }
  return requiredModules.toList();
}

kl::List<kl::Text> ModuleCollection::getModuleNames(const kl::Text& basePath) {
  auto path = kl::FilePath(basePath).replace_extension(""_t);
  kl::List<kl::Text> result;
  for (const auto& [name, mod]: modules) {
    if (name == path) {
      return {basePath};
    }
    if (mod->hasMain() && name.startsWith(basePath) && name.skip(basePath.size()).startsWith("/")) {
      result.add(name);
    }
  }
  return result;
}
