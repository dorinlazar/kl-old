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
  if (!kl::isDir(folderName)) {
    if (CMD.verbose) {
      kl::log(folderName, "folder doesn't exist. Skipping");
    }
    return;
  }
  auto folder = std::make_shared<Folder>(folderName, folderName, nullptr);
  folders.add(folderName, folder);
  uint32_t depth = kl::FilePath(folderName).folderDepth();
  kl::navigateTree(folderName, [folder, depth](const kl::FileInfo& file) -> kl::NavigateInstructions {
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
