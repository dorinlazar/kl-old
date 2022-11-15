#include "module.h"
#include "modulecollection.h"
#include "klbsettings.h"
#include "ff/codescanner.h"
using namespace kl::literals;

struct SourceFile {
  kl::List<kl::Text> localIncludes;
  kl::List<kl::Text> systemIncludes;
  bool hasMain = false;
  ModuleItem modItem;

public:
  SourceFile(ModuleItem mi) : modItem(mi) {}
  kl::Text extension() const { return modItem.extension(); }
  kl::DateTime timestamp() const { return modItem.timestamp(); }
};

struct HeaderFile {
  kl::List<kl::Text> localIncludes;
  kl::List<kl::Text> systemIncludes;
  kl::List<kl::Text> recursiveLocalHeaderDeps;
  kl::List<kl::Text> recursiveSystemHeaderDeps;
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

  kl::List<Module*> requiredModules;

  std::optional<ModuleItem> object;
  std::optional<ModuleItem> executable;
  std::optional<SourceFile> source;
  std::optional<HeaderFile> header;
};

Module::Module(ModuleCollection* parent, const kl::Text& seed) {
  d = std::make_unique<Module::ModuleInternals>();
  d->name = seed;
  d->parent = parent;
  d->sourcePath = CMD.SourceFolder().add(seed).replace_extension(""_t);
  d->buildPath = CMD.BuildFolder().add(seed).replace_extension(""_t);
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
    if (CMD.Verbose()) {
      kl::log("For module {} we have no header", d->name);
    }
    return;
  }
  if (CMD.Verbose()) {
    kl::log("For module {} header is {}", d->name, headerPath());
  }

  kl::SourceCodeScanner scanner(headerPath());
  d->header->systemIncludes =
      scanner.systemIncludes().transform<kl::Text>([](const kl::Text& tr) { return tr.copy(); });
  d->header->localIncludes = scanner.localIncludes().transform<kl::Text>(
      [this](const kl::Text& tr) { return d->parent->resolvePath(tr.copy(), this).fullPath(); });
}

void Module::_scanSource() {
  if (!d->source.has_value()) {
    if (CMD.Verbose()) {
      kl::log("For module {} we have no source", d->name);
    }
    return;
  }
  if (CMD.Verbose()) {
    kl::log("For module {} source is {}", d->name, sourcePath());
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
    CHECK(mod != nullptr, "Sanity check failed: search for module for {} failed.", deph);
    CHECK(mod->d->header.has_value(), "Sanity check failed: {} doesn't have a header?", mod->name());
    if (mod->d->header->recursiveLocalHeaderDeps.size() > 0) {
      resolved.add(mod->d->header->recursiveLocalHeaderDeps);
    } else {
      for (const auto& h: mod->d->header->localIncludes) {
        if (!resolved.has(h) && !processingQueue.has(h)) {
          processingQueue.push(h);
        }
      }
    }
    if (mod->d->header->recursiveSystemHeaderDeps.size() > 0) {
      resolvedSystem.add(mod->d->header->recursiveSystemHeaderDeps);
    } else {
      resolvedSystem.add(mod->d->header->systemIncludes);
    }
  }
  d->header->recursiveLocalHeaderDeps = resolved.toList();
  d->header->recursiveSystemHeaderDeps = resolvedSystem.toList();
}

void Module::updateModuleDependencies() {
  if (d->source.has_value()) {
    kl::Set<kl::Text> depSysHeaders;
    depSysHeaders.add(d->source->systemIncludes);
    kl::Set<kl::Text> depHeaders;
    for (const auto& inc: d->source->localIncludes) {
      auto mod = d->parent->getModule(inc);
      CHECK(mod != nullptr, "Sanity check failed: search for module for {} failed.", inc);
      CHECK(mod->d->header.has_value(), "Sanity check failed: {} doesn't have a header?", mod->name());
      depHeaders.add(mod->d->header->recursiveLocalHeaderDeps);
      depSysHeaders.add(mod->d->header->recursiveSystemHeaderDeps);
    }

    d->requiredModules = depHeaders.toList().transform<Module*>(
        [this](const kl::Text& include) { return d->parent->getModule(include).get(); });
    d->source->systemIncludes = depSysHeaders.toList();
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
  for (const auto& hmod: d->requiredModules) {
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
  for (const auto& hmod: d->requiredModules) {
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

kl::Text Module::buildFolder() const { return d->buildPath.folder_name(); }

bool Module::hasSource() const { return d->source.has_value(); }
bool Module::hasHeader() const { return d->header.has_value(); }

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
    for (auto m: mod->d->requiredModules) {
      if (!modules.has(m) && !processingQueue.has(m)) {
        processingQueue.push(m);
      }
    }
  }
  d->requiredModules = modules.toList();
}

const kl::List<Module*>& Module::requiredModules() const { return d->requiredModules; }
kl::List<kl::Text> Module::includeFolders() const {
  kl::Set<kl::Text> inc;
  for (const auto& mod: d->requiredModules) {
    inc.add(CMD.SourceFolder().add(mod->name()).folder_name());
  }
  return inc.toList();
}

kl::List<kl::Text> Module::recursiveSystemHeaders() const {
  kl::Set<kl::Text> inc;
  for (const auto& mod: d->requiredModules) {
    if (mod->d->source.has_value()) {
      inc.add(mod->d->source->systemIncludes);
    }
  }
  return inc.toList();
}

kl::List<kl::Text> Module::sourceSystemHeaders() const {
  if (d->source.has_value()) {
    return d->source->systemIncludes;
  }
  return {};
}
