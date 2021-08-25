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

void FSCache::addFolder(const kl::Text& folderName) {
  if (!kl::FileSystem::isDirectory(folderName)) {
    if (CMD.verbose) {
      kl::log(folderName, "folder doesn't exist. Skipping");
    }
    return;
  }
  auto folder = std::make_shared<Folder>(folderName, folderName, nullptr);
  uint32_t depth = kl::FilePath(folderName).folderDepth();
  kl::FileSystem::navigateTree(folderName, [folder, depth](const kl::FileInfo& file) -> kl::NavigateInstructions {
    auto f = file;
    f.path = file.path.remove_base_folder(depth);
    folder->addItem(f, file.path.fullPath());
    return kl::NavigateInstructions::Continue;
  });
  kl::Queue<std::shared_ptr<Folder>> fqueue;
  fqueue.push(folder);
  while (!fqueue.empty()) {
    auto f = fqueue.pop();
    all.add(f->fullPath().fullPath(), f);
    fqueue.push(f->getFolders());
  }
}

Folder* FSCache::getFolder(const kl::FilePath& name) {
  for (const auto& [fld, container]: all) {
    auto optfp = kl::FilePath(fld).hasFile(name);
    if (optfp.has_value()) {
      auto fp = *optfp;
      Folder* folder = container.get();
      while (fp.fullPath().size() > 0) {
        auto pfld = folder->getFolder(fp.baseFolder());
        if (pfld) {
          folder = pfld.get();
          fp = fp.remove_base_folder();
        } else {
          if (!folder->hasFile(fp.fullPath())) {
            return nullptr;
          }
        }
      }
      return folder;
    }
  }
  return nullptr;
}
