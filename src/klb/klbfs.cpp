#include "klbfs.h"
#include "klbsettings.h"

Folder::Folder(const kl::Text& name, const kl::Text& path, const Folder* parent)
    : _parent(parent), _name(name), _path(path) {}

void Folder::addItem(const kl::FileInfo& fi, const kl::Text& fullPath) {
  if (fi.path.folderName().size() == 0) {
    if (fi.type == kl::FileType::Directory) {
      _folders.add(fi.path.fullPath(), std::make_shared<Folder>(fi.path.fullPath(), fullPath, this));
    } else {
      _files.add(fi);
    }
  } else {
    auto fi2 = fi;
    auto baseFolder = fi.path.baseFolder();
    CHECK(_folders.has(baseFolder), "Sanity check: File in folder", baseFolder, "added, but folder not recorded");
    fi2.path = fi2.path.remove_base_folder();
    _folders[baseFolder]->addItem(fi2, fullPath);
  }
}

kl::ptr<Folder> Folder::getFolder(const kl::Text& name) { return _folders.get(name, nullptr); }
kl::List<kl::ptr<Folder>> Folder::getFolders() const { return _folders.values(); }
kl::ptr<Folder> Folder::createFolder(const kl::FilePath& fp) {
  if (fp.fullPath().size() == 0) {
    return nullptr;
  }
  kl::ptr<Folder> where = nullptr;
  kl::FilePath currentPath;
  for (const auto& fld: fp.breadcrumbs()) {
    currentPath = currentPath.add(fld);
    Folder* ptr = where != nullptr ? where.get() : this;
    where = ptr->getFolder(fld);
    if (where == nullptr) {
      where = std::make_shared<Folder>();
      ptr->_folders.add(fld, where);
    }
  }
  return where;
}

const kl::FilePath& Folder::fullPath() const { return _path; }
const kl::List<kl::FileInfo>& Folder::files() const { return _files; }

bool Folder::hasFile(const kl::Text& file) const {
  return _files.any([file](const auto& f) { return f.path.fileName() == file; });
}

std::ostream& Folder::write(std::ostream& os) const {
  os << "Folder: " << _name << " with ";
  if (_parent) {
    os << "parent: " << _parent->_name;
  } else {
    os << "no parent";
  }
  os << " FullPath: " << _path;
  os << "\nFolders: " << _folders.keys();
  os << "\nFiles: " << _files.transform<kl::Text>([](const kl::FileInfo& fi) { return fi.path.fileName(); });
  return os;
}

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
  if (!kl::FileSystem::isDirectory(folderName.fullPath())) {
    if (CMD.verbose) {
      kl::log(folderName, "folder doesn't exist. Skipping");
    }
    return nullptr;
  }
  auto folder = parent->createFolder(folderName);
  uint32_t depth = folderName.folderDepth();
  kl::FileSystem::navigateTree(folderName.fullPath(),
                               [folder, depth](const kl::FileInfo& file) -> kl::NavigateInstructions {
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
