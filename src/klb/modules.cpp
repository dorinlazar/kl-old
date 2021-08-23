#include "modules.h"
#include "klbsettings.h"

void ModuleCollection::discoverAll() {
  _scanModules();
  _updateModuleDependencies();
}

void ModuleCollection::_scanModules() {
  uint32_t srcDepth = kl::FilePath(CMD.sourceFolder).folderDepth();
  uint32_t bldDepth = kl::FilePath(CMD.buildFolder).folderDepth();
  for (const auto& [path, folder]: _cache->all) {
    if (path.startsWith(CMD.sourceFolder)) { // we process sources first
      auto moduleFolder = folder->fullPath().remove_base_folder(srcDepth);
      for (const auto& file: folder->files()) {
        auto mod = makeModule(moduleFolder.fullPath(), file.path.stem());
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

void ModuleCollection::_updateModuleDependencies() {}

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

std::shared_ptr<Module> ModuleCollection::makeModule(const kl::FilePath& folder, const kl::Text& stem) {
  kl::Text moduleName = kl::FilePath(folder.fullPath() + "/"_t + stem).fullPath();
  auto val = modules.get(moduleName, nullptr);
  if (!val) {
    val = std::make_shared<Module>(this, moduleName);
    modules.add(moduleName, val);
  }
  return val;
}

Module::Module(ModuleCollection* parent, const kl::Text& seed) : name(seed), parent(parent) {}

void Module::addFile(const kl::FileInfo& fi) {
  ModuleItem mi(fi);
  if (mi.type == ModuleItemType::Header) {
    header = mi;
  } else if (mi.type == ModuleItemType::Code) {
    source = mi;
  } else if (mi.type == ModuleItemType::Object) {
    object = mi;
  } else if (mi.type == ModuleItemType::Executable) {
    executable = mi;
  }
}

void ModuleItem::_updateModuleType() {
  if (extension.startsWith("h")) {
    type = ModuleItemType::Header;
  } else if (extension.startsWith("o")) {
    type = ModuleItemType::Object;
  } else if (extension.startsWith("c")) {
    type = ModuleItemType::Code;
  } else if (extension.size() == 0) {
    type = ModuleItemType::Executable;
  } else {
    type = ModuleItemType::Unknown;
  }
}

ModuleItem::ModuleItem(const kl::FileInfo& fi)
    : extension(fi.path.extension()), timestamp(fi.lastWrite), path(fi.path) {
  if (CMD.verbose) {
    kl::log("Adding module item", fi.path);
  }
  _updateModuleType();
}

ModuleItem::ModuleItem(const kl::FilePath& fullPath, kl::DateTime ts)
    : extension(fullPath.extension()), timestamp(ts), path(fullPath) {
  if (path.fullPath().startsWith(CMD.buildFolder)) {
    relPath = path.remove_base_folder(kl::FilePath(CMD.buildFolder).folderDepth());
  }
  _updateModuleType();
}

void Module::updateModuleInfo() {
  _updateComponentPaths();
  _scanHeader();
  _scanSource();
  _updateHeaderDependencies();
  _updateModuleDependencies();
  _updateIncludeFolders();
}

void Module::_updateComponentPaths() {
  if (source.has_value()) {
    source->relPath = kl::FilePath(name).replace_extension(source->path.extension());
    source->path = source->relPath.replace_base_folder(CMD.sourceFolder, 0);
  }
  if (header.has_value()) {
    header->relPath = kl::FilePath(name).replace_extension(header->path.extension());
    header->path = header->relPath.replace_base_folder(CMD.sourceFolder, 0);
  }
  if (object.has_value()) {
    object->relPath = kl::FilePath(name).replace_extension(object->path.extension());
    object->path = object->relPath.replace_base_folder(CMD.buildFolder, 0);
  }
  if (executable.has_value()) {
    executable->relPath = kl::FilePath(name).replace_extension(executable->path.extension());
    executable->path = executable->relPath.replace_base_folder(CMD.buildFolder, 0);
  }
}

struct IncludeInfo {
  kl::Text include;
  bool system = false;
};

std::optional<IncludeInfo> readInclude(const kl::Text& input) {
  auto t = input.skip(1).expectws("include");
  if (t.has_value() && t->size() >= 3) {
    auto inc = t->expectws("<");
    if (inc.has_value()) { // looking forward to seeing a ? operator here.
      auto res = inc->pos('>');
      if (res.has_value()) {
        return IncludeInfo{.include = inc->subpos(0, (*res) - 1).copy(), .system = true};
      }
    } else {
      inc = t->expectws("\"");
      if (inc.has_value()) {
        auto res = inc->pos('"');
        if (res.has_value()) {
          return IncludeInfo{.include = inc->subpos(0, (*res) - 1).copy(), .system = false};
        }
      }
    }
  }
  return {};
}

void Module::_scanHeader() {
  auto header = getHeader();
  if (!header.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", name, "we have no header");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", name, "header is", header->path);
  }

  auto lines = kl::readFile(header->path.fullPath()).splitLines();
  for (const auto& l: lines) {
    auto t = l.trimLeft();
    if (t.size() == 0) {
      continue;
    }
    if (t[0] == '#') {
      auto ii = readInclude(t);
      if (ii.has_value()) {
        if (ii->system) {
          headerSysIncludes.add(ii->include);
        } else {
          headerLocalIncludes.add(_resolveHeader(ii->include));
        }
      }
    }
  }
}

inline bool checkForMain(const kl::Text& txt) {
  auto t = txt.expect("int");
  if (t.has_value()) {
    t = t->expectws("main");
    if (t.has_value()) {
      return !t.has_value() || t->size() == 0 || t->expectws("(").has_value();
    }
  }
  return false;
}

void Module::_scanSource() {
  hasMain = false;
  auto source = getSource();
  if (!source.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", name, "we have no source");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", name, "source is", source->path);
  }

  auto lines = kl::readFile(source->path.fullPath()).splitLines();
  for (const auto& l: lines) {
    auto t = l.trimLeft();
    if (t.size() == 0) {
      continue;
    }
    if (t[0] == '#') {
      auto ii = readInclude(t);
      if (ii.has_value()) {
        if (ii->system) {
          systemIncludes.add(ii->include);
        } else {
          localIncludes.add(_resolveHeader(ii->include));
        }
      }
    } else if (!hasMain) {
      hasMain = checkForMain(t);
    }
  }
}

kl::Text Module::_resolveHeader(const kl::Text& inc) {
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

  auto modh = kl::FilePath(name).replace_extension(header->path.extension()).fullPath();

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
    object->timestamp = timestamp;
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

  kl::DateTime dt = object->timestamp;
  if (source->timestamp > dt) {
    return true;
  }
  for (const auto& mod: requiredModules) {
    auto hmod = parent->getModule(mod);
    CHECK(hmod != nullptr);
    auto hmodheader = hmod->getHeader();
    if (hmodheader.has_value() && hmodheader->timestamp > dt) {
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

  kl::DateTime dt = executable->timestamp;
  if (object->timestamp > dt) {
    return true;
  }
  for (const auto& mod: requiredModules) {
    auto hmod = parent->getModule(mod);
    CHECK(hmod != nullptr);
    auto hmodobject = hmod->getObject();
    if (hmodobject.has_value() && hmodobject->timestamp > dt) {
      return true;
    }
  }
  return false;
}

kl::Text Module::getObjectPath() const {
  if (object.has_value()) {
    return object->path.fullPath();
  }
  return kl::FilePath(name).replace_base_folder(CMD.buildFolder, 0).replace_extension("o"_t).fullPath();
}

kl::Text Module::getSourcePath() const {
  if (source.has_value()) [[likely]] {
    return source->path.fullPath();
  }
  return kl::FilePath(name).replace_base_folder(CMD.buildFolder, 0).replace_extension("cpp"_t).fullPath();
}

kl::Text Module::getExecutablePath() const {
  if (executable.has_value()) {
    return executable->path.fullPath();
  }
  return kl::FilePath(name).replace_base_folder(CMD.buildFolder, 0).replace_extension(""_t).fullPath();
}

kl::Text Module::getBuildFolder() const {
  return kl::FilePath(name).replace_base_folder(CMD.buildFolder, 0).folderName();
}
