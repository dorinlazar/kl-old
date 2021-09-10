#pragma once
#include "kl/kltext.h"
#include "kl/klfs.h"

class FSCache {
  kl::ptr<kl::Folder> _build;
  kl::ptr<kl::Folder> _source;
  kl::ptr<kl::Folder> _parent;

  kl::ptr<kl::Folder> _addFolder(const kl::Text& path);

public:
  FSCache(const kl::FilePath& source, const kl::FilePath& build);
  kl::List<kl::Folder*> getAllSubFolders(const kl::FilePath& base) const;
  kl::List<kl::Folder*> getAllSourceFolders() const;
  kl::List<kl::Folder*> getAllBuildFolders() const;

  kl::Folder* getFolder(const kl::FilePath& name) const;
  bool fileExists(const kl::FilePath& path) const;
};
