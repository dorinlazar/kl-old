#pragma once
#include "kl/kltext.h"
#include "kl/klfs.h"

struct Folder {
  const Folder* _parent = nullptr;
  kl::Text _name;
  kl::FilePath _path;
  kl::Dict<kl::Text, std::shared_ptr<Folder>> _folders;
  kl::List<kl::FileInfo> _files;

public:
  Folder() = default;
  Folder(const kl::Text& name, const kl::Text& path, const Folder* parent)
      : _parent(parent), _name(name), _path(path) {}
  void addItem(const kl::FileInfo& file, const kl::Text& path);

  std::shared_ptr<Folder> getFolder(const kl::Text& folder);
  kl::List<std::shared_ptr<Folder>> getFolders() const { return _folders.values(); }
  const kl::FilePath& fullPath() const { return _path; }
  const kl::List<kl::FileInfo>& files() const { return _files; }
  bool hasFile(const kl::Text& file) const;
  std::ostream& write(std::ostream& os) const;
};

struct FSCache {
  kl::Dict<kl::Text, std::shared_ptr<Folder>> all;

  void addFolder(const kl::Text& folderName);
  Folder* getFolder(const kl::FilePath& name) const;
  kl::List<Folder*> getAllSubFolders(const kl::FilePath& base) const;
};

inline std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Folder> l) { return l->write(os); }
inline std::ostream& operator<<(std::ostream& os, const Folder& l) { return l.write(os); }
