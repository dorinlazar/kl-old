#include "klbfs.h"
#include "klbsettings.h"
using namespace kl;

Folder* FSCache::get_folder(const kl::FilePath& name) const {
  auto where = m_parent;
  for (const auto& subfld: name.breadcrumbs()) {
    where = where->get_folder(subfld);
    if (where == nullptr) {
      return nullptr;
    }
  }
  return where.get();
}

bool FSCache::fileExists(const kl::FilePath& path) const {
  auto where = m_parent;
  for (const auto& subfld: path.folderName().splitByChar('/')) {
    where = where->get_folder(subfld);
    if (where == nullptr) {
      return false;
    }
  }
  return where->has_file(path.filename());
}

kl::List<Folder*> getSubfoldersRecursively(Folder* start) {
  kl::List<Folder*> res;
  kl::Queue<Folder*> queue;
  queue.push(start);
  while (!queue.empty()) {
    auto fld = queue.pop();
    res.add(fld);
    for (const auto& f: fld->get_folders()) {
      queue.push(f.get());
    }
  }
  return res;
}

kl::List<Folder*> FSCache::getAllSubFolders(const kl::FilePath& base) const {
  Folder* baseFolder = get_folder(base);
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
  auto folder = parent->create_folder(folderName);
  uint32_t depth = folderName.folderDepth();
  kl::FileSystem::navigate_tree(folderName.fullPath(),
                                [folder, depth](const kl::FileSystemEntryInfo& file) -> kl::NavigateInstructions {
                                  auto f = file;
                                  f.path = file.path.discardBaseFolder(depth);
                                  folder->add_item(f, file.path.fullPath());
                                  return kl::NavigateInstructions::Continue;
                                });

  return folder;
}

FSCache::FSCache(const kl::FilePath& source, const kl::FilePath& build) {
  m_parent = std::make_shared<Folder>("[BASE FOLDER]"_t, ""_t, nullptr);
  _source = _readFolder(source, m_parent);
  _build = _readFolder(build, m_parent);
}
