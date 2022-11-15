#include "klbfs.h"
#include "klbsettings.h"
using namespace kl;

Folder* FSCache::getFolder(const kl::FilePath& name) const {
  auto where = _parent;
  for (const auto& subfld: name.breadcrumbs()) {
    where = where->getFolder(subfld);
    if (where == nullptr) {
      return nullptr;
    }
  }
  return where.get();
}

bool FSCache::fileExists(const kl::FilePath& path) const {
  auto where = _parent;
  for (const auto& subfld: path.folderName().splitByChar('/')) {
    where = where->getFolder(subfld);
    if (where == nullptr) {
      return false;
    }
  }
  return where->hasFile(path.fileName());
}

kl::List<Folder*> getSubfoldersRecursively(Folder* start) {
  kl::List<Folder*> res;
  kl::Queue<Folder*> queue;
  queue.push(start);
  while (!queue.empty()) {
    auto fld = queue.pop();
    res.add(fld);
    for (const auto& f: fld->getFolders()) {
      queue.push(f.get());
    }
  }
  return res;
}

kl::List<Folder*> FSCache::getAllSubFolders(const kl::FilePath& base) const {
  Folder* baseFolder = getFolder(base);
  if (baseFolder) {
    return getSubfoldersRecursively(baseFolder);
  }
  return {};
}

// Temporary implementations;
kl::List<Folder*> FSCache::getAllSourceFolders() const {
  if (_source) {
    return getSubfoldersRecursively(_source.get());
  }
  return {};
}

kl::List<Folder*> FSCache::getAllBuildFolders() const {
  if (_build) {
    return getSubfoldersRecursively(_build.get());
  }
  return {};
}

kl::ptr<Folder> _readFolder(const kl::FilePath& folderName, kl::ptr<Folder> parent) {
  if (!kl::FileSystem::is_directory(folderName.fullPath())) {
    if (CMD.Verbose()) {
      kl::log("{} folder doesn't exist. Skipping", folderName.fullPath());
    }
    return nullptr;
  }
  auto folder = parent->createFolder(folderName);
  uint32_t depth = folderName.folderDepth();
  kl::FileSystem::navigate_tree(folderName.fullPath(),
                                [folder, depth](const kl::FileSystemEntryInfo& file) -> kl::NavigateInstructions {
                                  auto f = file;
                                  f.path = file.path.remove_base_folder(depth);
                                  folder->addItem(f, file.path.fullPath());
                                  return kl::NavigateInstructions::Continue;
                                });

  return folder;
}

FSCache::FSCache(const kl::FilePath& source, const kl::FilePath& build) {
  _parent = std::make_shared<Folder>("[BASE FOLDER]"_t, ""_t, nullptr);
  _source = _readFolder(source, _parent);
  _build = _readFolder(build, _parent);
}
