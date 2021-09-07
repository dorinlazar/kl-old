#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"

void ModuleCollection::discoverAll() {
  _scanAllModules();
  for (const auto& [name, mod]: modules) {
    mod->scanModuleRequirements();
  }
  for (const auto& [name, mod]: modules) {
    mod->updateModuleInfo();
  }
}

void ModuleCollection::discoverTests() {
  _scanModules({"tests"_t});
  for (const auto& [name, mod]: modules) {
    mod->scanModuleRequirements();
  }
  for (const auto& [name, mod]: modules) {
    mod->updateModuleInfo();
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

void ModuleCollection::_scanModules(const kl::List<kl::Text>& targets) {
  uint32_t srcDepth = CMD.sourceFolder.folderDepth();
  uint32_t bldDepth = CMD.buildFolder.folderDepth();

  kl::List<Folder*> folders;

  for (const auto& target: targets) {
    folders.add(_cache->getAllSubFolders(CMD.sourceFolder.add(target)));
  }

  for (auto folder: folders) {
    auto path = folder->fullPath();
    auto modFld = path.fullPath() == CMD.sourceFolder ? ""_t : folder->fullPath().remove_base_folder(srcDepth);
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
  kl::Text moduleName = kl::FilePath((folder.fullPath().size() ? folder.fullPath() : "."_t) + "/"_t + file)
                            .replace_extension(""_t)
                            .fullPath();
  return modules.get(moduleName, nullptr);
}

std::shared_ptr<Module> ModuleCollection::getModule(const kl::Text& file) const {
  kl::Text moduleName = kl::FilePath(file).replace_extension(""_t).fullPath();
  return modules.get(moduleName, nullptr);
}

std::shared_ptr<Module> ModuleCollection::getOrCreateModule(const kl::FilePath& folder, const kl::Text& stem) {
  kl::Text moduleName = kl::FilePath(folder.fullPath() + "/"_t + stem).fullPath();
  auto val = modules.get(moduleName, nullptr);
  if (!val) {
    val = std::make_shared<Module>(this, moduleName);
    modules.add(moduleName, val);
  }
  return val;
}
