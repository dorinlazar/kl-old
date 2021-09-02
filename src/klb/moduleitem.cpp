#include "moduleitem.h"
#include "klbsettings.h"

void ModuleItem::_updateModuleType() {
  _type = ModuleItemType::Unknown;

  if (_extension.size() > 0) {
    char exttype = _extension[0];
    if (exttype == 'h') {
      _type = ModuleItemType::Header;
    } else if (exttype == 'o') {
      _type = ModuleItemType::Object;
    } else if (exttype == 'c') {
      _type = ModuleItemType::Code;
    } else if (_extension == "exe"_t) {
      _type = ModuleItemType::Executable;
    }
  }
}

ModuleItem::ModuleItem(const kl::FileInfo& fi) : _timestamp(fi.lastWrite), _extension(fi.path.extension()) {
  if (CMD.verbose) {
    kl::log("Adding module item", fi.path);
  }
  _updateModuleType();
}

ModuleItem::ModuleItem(const kl::FilePath& fullPath, kl::DateTime ts)
    : _timestamp(ts), _extension(fullPath.extension()) {
  _updateModuleType();
}

kl::Text ModuleItem::extension() const { return _extension; }
ModuleItemType ModuleItem::type() const { return _type; }
kl::DateTime ModuleItem::timestamp() const { return _timestamp; }
void ModuleItem::setTimestamp(kl::DateTime ts) { _timestamp = ts; }
