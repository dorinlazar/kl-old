#include "module.h"
#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"
#include "depprocessor.h"

Module::Module(ModuleCollection* parent, const kl::Text& seed) : _name(seed), _parent(parent) {
  _sourcePath = CMD.sourceFolder.add(_name).replace_extension(""_t);
  _buildPath = CMD.buildFolder.add(_name).replace_extension(""_t);
}

void Module::addFile(const kl::FileInfo& fi) {
  ModuleItem mi(fi);
  switch (mi.type()) {
  case ModuleItemType::Header: header = mi; break;
  case ModuleItemType::Code: source = mi; break;
  case ModuleItemType::Object: object = mi; break;
  case ModuleItemType::Executable: executable = mi; break;
  default: break;
  }
}

void Module::scanModuleRequirements() {
  _scanHeader();
  _scanSource();
}

void Module::updateModuleInfo() {
  _updateHeaderDependencies();
  _updateModuleDependencies();
  _updateIncludeFolders();
}

void Module::_scanHeader() {
  if (!header.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", _name, "we have no header");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", _name, "header is", headerPath());
  }

  kl::SourceCodeScanner scanner(headerPath());
  headerSysIncludes.add(scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); }));
  headerLocalIncludes.add(scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return this->_resolveHeader(tr.copy()); }));
}

void Module::_scanSource() {
  hasMain = false;
  if (!source.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", _name, "we have no source");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", _name, "source is", sourcePath());
  }

  kl::SourceCodeScanner scanner(sourcePath());
  hasMain = scanner.hasMain();
  systemIncludes.add(scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); }));
  localIncludes.add(scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return this->_resolveHeader(tr.copy()); }));
}

kl::Text Module::_resolveHeader(const kl::Text& inc) {
  // TODO this should check against the filesystem && and add the module as well (when doing discovery)
  auto mod = _parent->getModule(kl::FilePath(_name).folderName(), inc);
  if (!mod) {
    mod = _parent->getModule(""_t, inc);
    CHECK(mod != nullptr, "Unable to identify module for", inc, "required from", _name);
  }
  auto text = kl::FilePath(mod->_name).replace_extension(kl::FilePath(inc).extension()).fullPath();
  if (CMD.verbose) {
    kl::log("RESOLVE:", inc, "=>", text);
  }
  return text;
}

void Module::_updateHeaderDependencies() {
  auto header = getHeader();
  if (!header.has_value()) {
    return;
  }

  auto modh = kl::FilePath(_name).replace_extension(header->extension()).fullPath();

  DependencyProcessor<kl::Text> proc;
  proc.add(_name);
  proc.process([this, &modh](const kl::Text& deph) {
    auto mod = _parent->getModule(""_t, deph);
    CHECK(mod != nullptr, "Unable to identify module for", deph, "required from", _name, "header dependency");
    if (mod->resolvedLocalHeaderDeps.size() > 0) {
      return mod->resolvedLocalHeaderDeps.toList().select([&modh](const kl::Text& h) { return h != modh; });
    }
    return mod->headerLocalIncludes.toList().select([&modh](const kl::Text& h) { return h != modh; });
  });
  resolvedLocalHeaderDeps = proc.result();
}

void Module::_updateModuleDependencies() {
  for (const auto& inc: localIncludes) {
    auto mod = _parent->getModule(""_t, inc);
    CHECK(mod != nullptr, "Unable to identify module for", inc, "required from", _name);
    requiredModules.add(kl::FilePath(inc).replace_extension(""_t).fullPath());
    requiredModules.add(mod->resolvedLocalHeaderDeps.toList().transform<kl::Text>(
        [](const kl::Text& t) { return kl::FilePath(t).replace_extension(""_t).fullPath(); }));
  }
}

void Module::_updateIncludeFolders() {
  for (const auto& mod: requiredModules) {
    includeFolders.add(CMD.sourceFolder.add(mod).folderName());
  }
}

void Module::updateObjectTimestamp(kl::DateTime timestamp) {
  if (object.has_value()) {
    object->setTimestamp(timestamp);
    return;
  }

  kl::FilePath path(objectPath());
  object = ModuleItem(path, timestamp);
}

bool Module::requiresBuild() const {
  if (!source.has_value()) {
    return false;
  }
  if (!object.has_value()) {
    return true;
  }

  kl::DateTime dt = object->timestamp();
  if (source->timestamp() > dt) {
    return true;
  }
  for (const auto& mod: requiredModules) {
    auto hmod = _parent->getModule(mod);
    CHECK(hmod != nullptr);
    auto hmodheader = hmod->getHeader();
    if (hmodheader.has_value() && hmodheader->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

bool Module::requiresLink() const {
  if (!hasMain) {
    return false;
  }
  if (!object.has_value() || !executable.has_value()) {
    return true;
  }

  kl::DateTime dt = executable->timestamp();
  if (object->timestamp() > dt) {
    return true;
  }
  for (const auto& mod: requiredModules) {
    auto hmod = _parent->getModule(mod);
    CHECK(hmod != nullptr);
    auto hmodobject = hmod->getObject();
    if (hmodobject.has_value() && hmodobject->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

kl::Text Module::headerPath() const {
  auto ext = header ? header->extension() : "h"_t;
  return _sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::sourcePath() const {
  auto ext = source ? source->extension() : "cpp"_t;
  return _sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::objectPath() const {
  auto ext = object ? object->extension() : "o"_t;
  return _buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::executablePath() const {
  auto ext = executable ? executable->extension() : "exe"_t;
  return _buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::buildFolder() const { return _buildPath.fullPath(); }

bool Module::hasSource() const { return source.has_value(); }

const kl::Text& Module::name() const { return _name; }