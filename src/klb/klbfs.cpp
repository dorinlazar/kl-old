#include "klbfs.h"
#include "klbsettings.h"

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

std::shared_ptr<Folder> Folder::getFolder(const kl::Text& name) { return _folders.get(name, nullptr); }

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
  bool isSource = name.fullPath().startsWith(CMD.sourceFolder);
  kl::ptr<Folder> where = isSource ? _source : _build;
  if (!where) {
    return nullptr;
  }
  auto fp = name.remove_base_folder(isSource ? kl::FilePath(CMD.sourceFolder).folderDepth()
                                             : kl::FilePath(CMD.buildFolder).folderDepth());

  auto breadcrumbs = fp.breadcrumbs();

  for (const auto& subfld: breadcrumbs) {
    where = where->getFolder(subfld);
  }
  return where.get();
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

kl::ptr<Folder> _readFolder(const kl::Text& folderName) {
  if (!kl::FileSystem::isDirectory(folderName)) {
    if (CMD.verbose) {
      kl::log(folderName, "folder doesn't exist. Skipping");
    }
    return nullptr;
  }
  auto folder = std::make_shared<Folder>(folderName, folderName, nullptr);
  uint32_t depth = kl::FilePath(folderName).folderDepth();
  kl::FileSystem::navigateTree(folderName, [folder, depth](const kl::FileInfo& file) -> kl::NavigateInstructions {
    auto f = file;
    f.path = file.path.remove_base_folder(depth);
    folder->addItem(f, file.path.fullPath());
    return kl::NavigateInstructions::Continue;
  });

  return folder;
}

FSCache::FSCache(const kl::Text& source, const kl::Text& build) {
  _source = _readFolder(source);
  _build = _readFolder(build);
}
