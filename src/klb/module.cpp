#include "module.h"
#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"

struct SourceFile {
  kl::List<kl::Text> localIncludes;
  kl::List<kl::Text> systemIncludes;
  kl::List<Module*> requiredModules;
  bool hasMain = false;
  ModuleItem modItem;

public:
  SourceFile(ModuleItem mi) : modItem(mi) {}
  kl::Text extension() const { return modItem.extension(); }
  kl::DateTime timestamp() const { return modItem.timestamp(); }
};

struct HeaderFile {
  kl::List<kl::Text> headerLocalIncludes;
  kl::List<kl::Text> headerSysIncludes;
  kl::List<kl::Text> resolvedLocalHeaderDeps;
  kl::List<kl::Text> resolvedSystemHeaderDeps;
  ModuleItem modItem;

public:
  HeaderFile(ModuleItem mi) : modItem(mi) {}
  kl::Text extension() const { return modItem.extension(); }
  kl::DateTime timestamp() const { return modItem.timestamp(); }
};

struct Module::ModuleInternals {
  kl::Text name;
  ModuleCollection* parent;
  kl::FilePath buildPath;
  kl::FilePath sourcePath;

  std::optional<ModuleItem> object;
  std::optional<ModuleItem> executable;
  std::optional<SourceFile> source;
  std::optional<HeaderFile> header;
};

Module::Module(ModuleCollection* parent, const kl::Text& seed) {
  d = std::make_unique<Module::ModuleInternals>();
  d->name = seed;
  d->parent = parent;
  d->sourcePath = CMD.sourceFolder.add(seed).replace_extension(""_t);
  d->buildPath = CMD.buildFolder.add(seed).replace_extension(""_t);
}

Module::~Module() {}

void Module::addFile(const kl::FileSystemEntryInfo& fi) {
  ModuleItem mi(fi);
  switch (mi.type()) {
  case ModuleItemType::Header: d->header = mi; break;
  case ModuleItemType::Code: d->source = mi; break;
  case ModuleItemType::Object: d->object = mi; break;
  case ModuleItemType::Executable: d->executable = mi; break;
  default: break;
  }
}

void Module::readDirectRequirements() {
  _scanHeader();
  _scanSource();
}

void Module::_scanHeader() {
  if (!d->header.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", d->name, "we have no header");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", d->name, "header is", headerPath());
  }

  kl::SourceCodeScanner scanner(headerPath());
  d->header->headerSysIncludes =
      scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); });
  d->header->headerLocalIncludes = scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return d->parent->resolvePath(tr.copy(), this).fullPath(); });
}

void Module::_scanSource() {
  if (!d->source.has_value()) {
    if (CMD.verbose) {
      kl::log("For module", d->name, "we have no source");
    }
    return;
  }
  if (CMD.verbose) {
    kl::log("For module", d->name, "source is", sourcePath());
  }

  kl::SourceCodeScanner scanner(sourcePath());
  d->source->hasMain = scanner.hasMain();
  d->source->systemIncludes =
      scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); });
  d->source->localIncludes = scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return d->parent->resolvePath(tr.copy(), this).fullPath(); });
}

void Module::updateHeaderDependencies() {
  if (!d->header.has_value()) {
    return;
  }

  auto modh = kl::FilePath(d->name).replace_extension(d->header->extension()).fullPath();

  kl::Set<kl::Text> resolved;
  kl::Set<kl::Text> resolvedSystem;
  kl::Queue<kl::Text> processingQueue;
  processingQueue.push(modh);
  while (!processingQueue.empty()) {
    auto deph = processingQueue.pop();
    resolved.add(deph);
    auto mod = d->parent->getModule(""_t, deph);
    CHECK(mod->d->header.has_value(), "Sanity check failed:", mod->name(), "doesn't have a header?");
    if (mod->d->header->resolvedLocalHeaderDeps.size() > 0) {
      resolved.add(mod->d->header->resolvedLocalHeaderDeps);
    } else {
      for (const auto& h: mod->d->header->headerLocalIncludes) {
        if (!resolved.has(h) && !processingQueue.has(h)) {
          processingQueue.push(h);
        }
      }
    }
    if (mod->d->header->resolvedSystemHeaderDeps.size() > 0) {
      resolvedSystem.add(mod->d->header->resolvedSystemHeaderDeps);
    } else {
      resolvedSystem.add(mod->d->header->headerSysIncludes);
    }
  }
  d->header->resolvedLocalHeaderDeps = resolved.toList();
  d->header->resolvedSystemHeaderDeps = resolvedSystem.toList();
}

void Module::updateModuleDependencies() {
  if (d->source.has_value()) {
    kl::Set<kl::Text> depHeaders;
    for (const auto& inc: d->source->localIncludes) {
      auto mod = d->parent->getModule(inc);
      CHECK(mod->d->header.has_value(), "Sanity check failed:", mod->name(), "doesn't have a header?");
      depHeaders.add(mod->d->header->resolvedLocalHeaderDeps);
    }

    d->source->requiredModules = depHeaders.toList().transform<Module*>(
        [this](const kl::Text& include) { return d->parent->getModule(include).get(); });
  }
}

void Module::updateObjectTimestamp(kl::DateTime timestamp) {
  if (d->object.has_value()) {
    d->object->setTimestamp(timestamp);
    return;
  }

  kl::FilePath path(objectPath());
  d->object = ModuleItem(path, timestamp);
}

bool Module::requiresBuild() const {
  if (!d->source.has_value()) {
    return false;
  }
  if (!d->object.has_value()) {
    return true;
  }

  kl::DateTime dt = d->object->timestamp();
  if (d->source->timestamp() > dt) {
    return true;
  }
  for (const auto& hmod: d->source->requiredModules) {
    CHECK(hmod != nullptr);
    const auto& hmodheader = hmod->d->header;
    if (hmodheader.has_value() && hmodheader->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

bool Module::requiresLink() const {
  if (!hasMain()) {
    return false;
  }
  if (!d->object.has_value() || !d->executable.has_value()) {
    return true;
  }

  kl::DateTime dt = d->executable->timestamp();
  if (d->object->timestamp() > dt) {
    return true;
  }
  for (const auto& hmod: d->source->requiredModules) {
    const auto& hmodobject = hmod->d->object;
    if (hmodobject.has_value() && hmodobject->timestamp() > dt) {
      return true;
    }
  }
  return false;
}

kl::Text Module::headerPath() const {
  auto ext = d->header ? d->header->extension() : "h"_t;
  return d->sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::sourcePath() const {
  auto ext = d->source ? d->source->extension() : "cpp"_t;
  return d->sourcePath.replace_extension(ext).fullPath();
}

kl::Text Module::objectPath() const {
  auto ext = d->object ? d->object->extension() : "o"_t;
  return d->buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::executablePath() const {
  auto ext = d->executable ? d->executable->extension() : "exe"_t;
  return d->buildPath.replace_extension(ext).fullPath();
}

kl::Text Module::buildFolder() const { return d->buildPath.fullPath(); }

bool Module::hasSource() const { return d->source.has_value(); }

const kl::Text& Module::name() const { return d->name; }
bool Module::hasMain() const { return d->source.has_value() && d->source->hasMain; }

void Module::recurseModuleDependencies() {
  kl::Set<Module*> modules;
  kl::Queue<Module*> processingQueue;
  if (!d->source.has_value()) {
    return;
  }
  processingQueue.push(this);
  while (!processingQueue.empty()) {
    auto mod = processingQueue.pop();
    modules.add(mod);
    if (mod->d->source.has_value()) {
      for (auto m: mod->d->source->requiredModules) {
        if (!modules.has(m) && !processingQueue.has(m)) {
          processingQueue.push(m);
        }
      }
    }
  }
  d->source->requiredModules = modules.toList();
}

const kl::List<Module*>& Module::requiredModules() const {
  static kl::List<Module*> empty; // TODO this is an awful trick. Fix this.
  return d->source.has_value() ? d->source->requiredModules : empty;
}
kl::List<kl::Text> Module::includeFolders() const {
  kl::Set<kl::Text> inc;
  if (d->source.has_value()) {
    for (const auto& mod: d->source->requiredModules) {
      inc.add(CMD.sourceFolder.add(mod->name()).folderName());
    }
  }
  return inc.toList();
}
