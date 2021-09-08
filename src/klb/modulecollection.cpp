#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"

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
  uint32_t srcDepth = CMD.sourceFolder.folderDepth();
  uint32_t bldDepth = CMD.buildFolder.folderDepth();
  for (auto folder: _cache->getAllSourceFolders()) {
    auto path = folder->fullPath();
    auto modFld = path == CMD.sourceFolder ? ""_t : path.remove_base_folder(srcDepth);
    for (const auto& file: folder->files()) {
      auto mod = getOrCreateModule(modFld, file.path.stem());
      mod->addFile(file);
    }
  }
  for (auto folder: _cache->getAllBuildFolders()) {
    auto path = folder->fullPath();
    auto moduleFolder = path == CMD.buildFolder ? ""_t : path.remove_base_folder(bldDepth);
    for (const auto& file: folder->files()) {
      auto mod = getModule(moduleFolder.fullPath(), file.path.stem());
      if (mod) {
        mod->addFile(file);
      }
    }
  }
}

std::shared_ptr<Module> ModuleCollection::getModule(const kl::FilePath& folder, const kl::Text& file) const {
  kl::Text moduleName = folder.add(file).replace_extension(""_t).fullPath();
  return modules[moduleName];
}

std::shared_ptr<Module> ModuleCollection::getModule(const kl::Text& file) const {
  kl::Text moduleName = kl::FilePath(file).replace_extension(""_t).fullPath();
  return modules[moduleName];
}

std::shared_ptr<Module> ModuleCollection::getOrCreateModule(const kl::FilePath& folder, const kl::Text& stem) {
  kl::Text moduleName = folder.add(stem).fullPath();
  auto val = modules.get(moduleName, nullptr);
  if (!val) {
    val = std::make_shared<Module>(this, moduleName);
    modules.add(moduleName, val);
  }
  return val;
}

kl::FilePath ModuleCollection::resolvePath(const kl::Text& name, Module* origin) const {
  auto path = CMD.sourceFolder.add(name);
  if (!_cache->fileExists(path)) {
    path = kl::FilePath(CMD.sourceFolder.add(origin->name()).folderName()).add(name);
    CHECK(_cache->fileExists(path), "Unable to locate dependency", name, "included in module", origin->name(),
          ". Tried:", CMD.sourceFolder.add(name), path);
  }
  return path.remove_base_folder(CMD.sourceFolder.folderDepth());
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
        FATAL("Unable to identify target", file);
      }
      if (fp.extension() == "o"_t) {
        requiredModules.add(mod.get());
      } else if (mod->hasMain() && (fp.extension() == "exe"_t || fp.extension().size() == 0)) {
        requiredModules.add(mod->requiredModules());
      } else {
        FATAL("Don't know how to build", file);
      }
    }
  }
  return requiredModules.toList();
}

kl::List<Module*> ModuleCollection::getExecutables(const kl::Text& basePath) {
  kl::List<Module*> result;
  for (const auto& [name, mod]: modules) {
    if (mod->hasMain() && name.startsWith(basePath) && name.skip(basePath.size()).startsWith("/")) {
      result.add(mod.get());
    }
  }
  return result;
}
