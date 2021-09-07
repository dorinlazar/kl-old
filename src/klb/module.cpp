#include "module.h"
#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"

Module::Module(ModuleCollection* parent, const kl::Text& seed) : _name(seed), _parent(parent) {
  _sourcePath = CMD.sourceFolder.add(_name).replace_extension(""_t);
  _buildPath = CMD.buildFolder.add(_name).replace_extension(""_t);
}

void Module::addFile(const kl::FileInfo& fi) {
  ModuleItem mi(fi);
  switch (mi.type()) {
  case ModuleItemType::Header: _header = mi; break;
  case ModuleItemType::Code: _source = mi; break;
  case ModuleItemType::Object: _object = mi; break;
  case ModuleItemType::Executable: _executable = mi; break;
  default: break;
  }
}

void Module::readDirectRequirements() {
  _scanHeader();
  _scanSource();
}

void Module::_scanHeader() {
  if (!_header.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", _name, "we have no header");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", _name, "header is", headerPath());
  }

  kl::SourceCodeScanner scanner(headerPath());
  _headerSysIncludes = scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); });
  _headerLocalIncludes = scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return _parent->resolvePath(tr.copy(), this).fullPath(); });
}

void Module::_scanSource() {
  _hasMain = false;
  if (!_source.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", _name, "we have no source");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", _name, "source is", sourcePath());
  }

  kl::SourceCodeScanner scanner(sourcePath());
  _hasMain = scanner.hasMain();
  _systemIncludes = scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); });
  _localIncludes = scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return _parent->resolvePath(tr.copy(), this).fullPath(); });
}

void Module::updateHeaderDependencies() {
  if (!_header.has_value()) {
    return;
  }

  auto modh = kl::FilePath(_name).replace_extension(_header->extension()).fullPath();

  kl::Set<kl::Text> resolved;
  kl::Queue<kl::Text> processingQueue;
  processingQueue.push(modh);
  while (!processingQueue.empty()) {
    auto deph = processingQueue.pop();
    resolved.add(deph);
    auto mod = _parent->getModule(""_t, deph);
    if (mod->_resolvedLocalHeaderDeps.size() > 0) {
      resolved.add(mod->_resolvedLocalHeaderDeps);
    } else {
      for (const auto& h: mod->_headerLocalIncludes) {
        if (!resolved.has(h) && !processingQueue.has(h)) {
          processingQueue.push(h);
        }
      }
    }
  }
  _resolvedLocalHeaderDeps = resolved.toList();
}

void Module::updateModuleDependencies() {
  if (_source.has_value()) {
    kl::Set<kl::Text> depHeaders;
    for (const auto& inc: _localIncludes) {
      depHeaders.add(_parent->getModule(inc)->_resolvedLocalHeaderDeps);
    }

    _requiredModules = depHeaders.toList().transform<Module*>(
        [this](const kl::Text& include) { return _parent->getModule(include).get(); });
  }
}

void Module::updateObjectTimestamp(kl::DateTime timestamp) {
  if (_object.has_value()) {
    _object->setTimestamp(timestamp);
    return;
  }

  kl::FilePath path(objectPath());
  _object = ModuleItem(path, timestamp);
}

bool Module::requiresBuild() const {
  if (!_source.has_value()) {
    return false;
  }
  if (!_object.has_value()) {
    return true;
  }

  kl::DateTime dt = _object->timestamp();
  if (_source->timestamp() > dt) {
    return true;
  }
  for (const auto& hmod: _requiredModules) {
    CHECK(hmod != nullptr);
    auto hmodheader = hmod->_header;
    if (hmodheader.has_value() && hmodheader->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

bool Module::requiresLink() const {
  if (!_hasMain) {
    return false;
  }
  if (!_object.has_value() || !_executable.has_value()) {
    return true;
  }

  kl::DateTime dt = _executable->timestamp();
  if (_object->timestamp() > dt) {
    return true;
  }
  for (const auto& hmod: _requiredModules) {
    auto hmodobject = hmod->_object;
    if (hmodobject.has_value() && hmodobject->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

kl::Text Module::headerPath() const {
  auto ext = _header ? _header->extension() : "h"_t;
  return _sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::sourcePath() const {
  auto ext = _source ? _source->extension() : "cpp"_t;
  return _sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::objectPath() const {
  auto ext = _object ? _object->extension() : "o"_t;
  return _buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::executablePath() const {
  auto ext = _executable ? _executable->extension() : "exe"_t;
  return _buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::buildFolder() const { return _buildPath.fullPath(); }

bool Module::hasSource() const { return _source.has_value(); }

const kl::Text& Module::name() const { return _name; }
bool Module::hasMain() const { return _hasMain; }

void Module::recurseModuleDependencies() {
  kl::Set<Module*> modules;
  kl::Queue<Module*> processingQueue;
  processingQueue.push(this);
  while (!processingQueue.empty()) {
    auto mod = processingQueue.pop();
    modules.add(mod);
    for (auto m: mod->_requiredModules) {
      if (!modules.has(m) && !processingQueue.has(m)) {
        processingQueue.push(m);
      }
    }
  }
  _requiredModules = modules.toList();
}

const kl::List<Module*>& Module::requiredModules() const { return _requiredModules; }
kl::List<kl::Text> Module::includeFolders() const {
  kl::Set<kl::Text> inc;
  for (const auto& mod: _requiredModules) {
    inc.add(CMD.sourceFolder.add(mod->name()).folderName());
  }
  return inc.toList();
}
