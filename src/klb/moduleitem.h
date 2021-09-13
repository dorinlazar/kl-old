#pragma once
#include "kl/klfs.h"

enum class ModuleItemType { Unknown, Header, Code, Object, Executable };

class ModuleItem {
  kl::DateTime _timestamp;
  ModuleItemType _type;
  kl::Text _extension;

  void _updateModuleType();

public:
  ModuleItem(const kl::FileSystemEntryInfo& fi);
  ModuleItem(const kl::FilePath& fullPath, kl::DateTime timestamp);
  kl::Text extension() const;
  ModuleItemType type() const;

  kl::DateTime timestamp() const;
  void setTimestamp(kl::DateTime ts);
};
