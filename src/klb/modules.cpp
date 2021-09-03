#include "modules.h"
#include "klbsettings.h"
#include "ff/codescanner.h"

void ModuleCollection::discoverAll() { _scanAllModules(); }

void ModuleCollection::discoverTests() { _scanModules({"tests"_t}); }

void ModuleCollection::_scanAllModules() {
  uint32_t srcDepth = kl::FilePath(CMD.sourceFolder).folderDepth();
  uint32_t bldDepth = kl::FilePath(CMD.buildFolder).folderDepth();
  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.sourceFolder)) { // we process sources first
      auto moduleFolder = path == CMD.sourceFolder ? ""_t : folder->fullPath().remove_base_folder(srcDepth);
      for (const auto& file: folder->files()) {
        auto mod = getOrCreateModule(moduleFolder.fullPath(), file.path.stem());
        mod->addFile(file);
      }
    }
  }
  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.buildFolder)) { // we process build output second
      auto moduleFolder = folder->fullPath().remove_base_folder(bldDepth);
      for (const auto& file: folder->files()) {
        auto mod = getModule(moduleFolder.fullPath(), file.path.stem());
        if (mod) {
          mod->addFile(file);
        }
      }
    }
  }
}

static kl::List<Folder*> _getAllFolders(Folder* folder) {
  kl::List<Folder*> res;
  kl::Queue<Folder*> queue;
  queue.push(folder);
  while (!queue.empty()) {
    auto fld = queue.pop();
    res.add(fld);
    for (const auto& f: fld->getFolders()) {
      queue.push(f.get());
    }
  }
  return res;
}

void ModuleCollection::_scanModules(const kl::List<kl::Text>& targets) {
  uint32_t srcDepth = kl::FilePath(CMD.sourceFolder).folderDepth();
  uint32_t bldDepth = kl::FilePath(CMD.buildFolder).folderDepth();

  kl::List<Folder*> folders;

  for (const auto& target: targets) {
    kl::FilePath fp(CMD.sourceFolder + "/" + target);
    auto folder = _cache->getFolder(fp);
    if (folder) {
      kl::log("we're building a folder:", target, folder->fullPath());
      folders.add(_getAllFolders(folder));
    } else {
      kl::log("target not found as folder:", target);
    }
  }

  for (auto folder: folders) {
    auto moduleFolder = folder->fullPath().remove_base_folder(srcDepth);
    for (const auto& file: folder->files()) {
      auto mod = getOrCreateModule(moduleFolder.fullPath(), file.path.stem());
      mod->addFile(file);
    }
  }

  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.buildFolder)) { // we process build output second
      auto moduleFolder = folder->fullPath().remove_base_folder(bldDepth);
      for (const auto& file: folder->files()) {
        auto mod = getModule(moduleFolder.fullPath(), file.path.stem());
        if (mod) {
          mod->addFile(file);
        }
      }
    }
  }
}

std::shared_ptr<Module> ModuleCollection::getModule(const kl::FilePath& folder, const kl::Text& file) const {
  kl::Text moduleName = kl::FilePath((folder.fullPath().size() ? folder.fullPath() : "."_t) + "/"_t + file)
                            .replace_extension(""_t)
                            .fullPath();
  return modules.get(moduleName, nullptr);
}

std::shared_ptr<Module> ModuleCollection::getModule(const kl::Text& file) const {
  kl::Text moduleName = kl::FilePath(file).replace_extension(""_t).fullPath();
  return modules.get(moduleName, nullptr);
}

std::shared_ptr<Module> ModuleCollection::getOrCreateModule(const kl::FilePath& folder, const kl::Text& stem) {
  kl::Text moduleName = kl::FilePath(folder.fullPath() + "/"_t + stem).fullPath();
  auto val = modules.get(moduleName, nullptr);
  if (!val) {
    val = std::make_shared<Module>(this, moduleName);
    modules.add(moduleName, val);
  }
  return val;
}

Module::Module(ModuleCollection* parent, const kl::Text& seed)
    : name(seed), parent(parent), _buildPath(name), _sourcePath(name) {
  _sourcePath = _sourcePath.replace_base_folder(CMD.sourceFolder, 0).replace_extension(""_t);
  _buildPath = _buildPath.replace_base_folder(CMD.buildFolder, 0).replace_extension(""_t);
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

void Module::updateModuleInfo() {
  _scanHeader();
  _scanSource();
  _updateHeaderDependencies();
  _updateModuleDependencies();
  _updateIncludeFolders();
}

void Module::_scanHeader() {
  if (!header.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", name, "we have no header");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", name, "header is", getHeaderPath());
  }

  kl::SourceCodeScanner scanner(getHeaderPath());
  headerSysIncludes.add(scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); }));
  headerLocalIncludes.add(scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return this->_resolveHeader(tr.copy()); }));
}

void Module::_scanSource() {
  hasMain = false;
  if (!source.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", name, "we have no source");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", name, "source is", getSourcePath());
  }

  kl::SourceCodeScanner scanner(getSourcePath());
  hasMain = scanner.hasMain();
  systemIncludes.add(scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); }));
  localIncludes.add(scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return this->_resolveHeader(tr.copy()); }));
}

kl::Text Module::_resolveHeader(const kl::Text& inc) {
  // TODO this should check against the filesystem && and add the module as well (when doing discovery)
  auto mod = parent->getModule(kl::FilePath(name).folderName(), inc);
  if (!mod) {
    mod = parent->getModule(""_t, inc);
    CHECK(mod != nullptr, "Unable to identify module for", inc, "required from", name);
  }
  auto text = kl::FilePath(mod->name).replace_extension(kl::FilePath(inc).extension()).fullPath();
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

  auto modh = kl::FilePath(name).replace_extension(header->extension()).fullPath();

  kl::Queue<kl::Text> toProcess;
  toProcess.push(headerLocalIncludes.toList());

  while (!toProcess.empty()) {
    auto deph = toProcess.pop();
    resolvedLocalHeaderDeps.add(deph);
    auto mod = parent->getModule(""_t, deph);
    CHECK(mod != nullptr, "Unable to identify module for", deph, "required from", name, "header dependency");
    if (mod->resolvedLocalHeaderDeps.size() > 0) {
      for (const auto& h: mod->headerLocalIncludes) {
        if (h != modh) {
          resolvedLocalHeaderDeps.add(h);
        }
      }
    } else {
      for (const auto& h: mod->headerLocalIncludes) {
        if (!toProcess.has(h) && !resolvedLocalHeaderDeps.has(h) && h != modh) {
          toProcess.push(h);
        }
      }
    }
  }
}

void Module::_updateModuleDependencies() {
  for (const auto& inc: localIncludes) {
    auto mod = parent->getModule(""_t, inc);
    CHECK(mod != nullptr, "Unable to identify module for", inc, "required from", name);
    requiredModules.add(kl::FilePath(inc).replace_extension(""_t).fullPath());
    requiredModules.add(mod->resolvedLocalHeaderDeps.toList().transform<kl::Text>(
        [](const kl::Text& t) { return kl::FilePath(t).replace_extension(""_t).fullPath(); }));
  }
}

void Module::_updateIncludeFolders() {
  for (const auto& mod: requiredModules) {
    includeFolders.add(kl::FilePath(mod).replace_base_folder(CMD.sourceFolder, 0).folderName());
  }
}

void Module::updateObjectTimestamp(kl::DateTime timestamp) {
  if (object.has_value()) {
    object->setTimestamp(timestamp);
    return;
  }

  kl::FilePath path(getObjectPath());
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
    auto hmod = parent->getModule(mod);
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
    auto hmod = parent->getModule(mod);
    CHECK(hmod != nullptr);
    auto hmodobject = hmod->getObject();
    if (hmodobject.has_value() && hmodobject->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

kl::Text Module::getHeaderPath() const {
  auto ext = header ? header->extension() : "h"_t;
  return _sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::getSourcePath() const {
  auto ext = source ? source->extension() : "cpp"_t;
  return _sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::getObjectPath() const {
  auto ext = object ? object->extension() : "o"_t;
  return _buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::getExecutablePath() const {
  auto ext = executable ? executable->extension() : "exe"_t;
  return _buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::getBuildFolder() const { return _buildPath.fullPath(); }

bool Module::hasSource() const { return source.has_value(); }