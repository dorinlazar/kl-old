#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"

void ModuleCollection::discoverAll() { _scanAllModules(); }

void ModuleCollection::discoverTests() { _scanModules({"tests"_t}); }

void ModuleCollection::_scanAllModules() {
  uint32_t srcDepth = kl::FilePath(CMD.sourceFolder).folderDepth();
  uint32_t bldDepth = kl::FilePath(CMD.buildFolder).folderDepth();
  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.sourceFolder)) { // we process sources first
      auto moduleFolder = path == CMD.sourceFolder ? ""_t : folder->fullPath().remove_base_folder(srcDepth);
      for (const auto& file: folder->files()) {
        auto mod = getOrCreateModule(moduleFolder.fullPath(), file.path.stem());
        mod->addFile(file);
      }
    }
  }
  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.buildFolder)) { // we process build output second
      auto moduleFolder = folder->fullPath().remove_base_folder(bldDepth);
      for (const auto& file: folder->files()) {
        auto mod = getModule(moduleFolder.fullPath(), file.path.stem());
        if (mod) {
          mod->addFile(file);
        }
      }
    }
  }
}

static kl::List<Folder*> _getAllFolders(Folder* folder) {
  kl::List<Folder*> res;
  kl::Queue<Folder*> queue;
  queue.push(folder);
  while (!queue.empty()) {
    auto fld = queue.pop();
    res.add(fld);
    for (const auto& f: fld->getFolders()) {
      queue.push(f.get());
    }
  }
  return res;
}

void ModuleCollection::_scanModules(const kl::List<kl::Text>& targets) {
  uint32_t srcDepth = kl::FilePath(CMD.sourceFolder).folderDepth();
  uint32_t bldDepth = kl::FilePath(CMD.buildFolder).folderDepth();

  kl::List<Folder*> folders;

  for (const auto& target: targets) {
    kl::FilePath fp(CMD.sourceFolder + "/" + target);
    auto folder = _cache->getFolder(fp);
    if (folder) {
      kl::log("we're building a folder:", target, folder->fullPath());
      folders.add(_getAllFolders(folder));
    } else {
      kl::log("target not found as folder:", target);
    }
  }

  for (auto folder: folders) {
    auto moduleFolder = folder->fullPath().remove_base_folder(srcDepth);
    for (const auto& file: folder->files()) {
      auto mod = getOrCreateModule(moduleFolder.fullPath(), file.path.stem());
      mod->addFile(file);
    }
  }

  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.buildFolder)) { // we process build output second
      auto moduleFolder = folder->fullPath().remove_base_folder(bldDepth);
      for (const auto& file: folder->files()) {
        auto mod = getModule(moduleFolder.fullPath(), file.path.stem());
        if (mod) {
          mod->addFile(file);
        }
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
