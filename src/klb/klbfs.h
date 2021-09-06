#pragma once
#include "kl/kltext.h"
#include "kl/klfs.h"

struct Folder {
  const Folder* _parent = nullptr;
  kl::Text _name;
  kl::FilePath _path;
  kl::Dict<kl::Text, kl::ptr<Folder>> _folders;
  kl::List<kl::FileInfo> _files;

public:
  Folder() = default;
  Folder(const kl::Text& name, const kl::Text& path, const Folder* parent);
  void addItem(const kl::FileInfo& file, const kl::Text& path);

  kl::ptr<Folder> getFolder(const kl::Text& folder);
  kl::List<kl::ptr<Folder>> getFolders() const;
  kl::ptr<Folder> createFolder(const kl::FilePath& path);
  const kl::FilePath& fullPath() const;
  const kl::List<kl::FileInfo>& files() const;
  bool hasFile(const kl::Text& file) const;
  std::ostream& write(std::ostream& os) const;
};

class FSCache {
  kl::ptr<Folder> _build;
  kl::ptr<Folder> _source;
  kl::ptr<Folder> _parent;

  kl::ptr<Folder> _addFolder(const kl::Text& path);

public:
  FSCache(const kl::FilePath& source, const kl::FilePath& build);
  Folder* getFolder(const kl::FilePath& name) const;
  kl::List<Folder*> getAllSubFolders(const kl::FilePath& base) const;
  kl::List<Folder*> getAllSourceFolders() const;
  kl::List<Folder*> getAllBuildFolders() const;
};

inline std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Folder> l) { return l->write(os); }
inline std::ostream& operator<<(std::ostream& os, const Folder& l) { return l.write(os); }
