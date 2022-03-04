#pragma once

#include "modulecollection.h"
#include <fstream>

class BuildStrategy {
  ModuleCollection* _modules;

public:
  BuildStrategy(ModuleCollection* coll);
  virtual ~BuildStrategy();
  virtual void build(Module* module) = 0;
  virtual void link(Module* module) = 0;
  virtual void run(Module* module) = 0;
  virtual bool execute() = 0;
};

struct ExecutionStrategyImpl;

class DefaultBuildStrategy final : public BuildStrategy {
  kl::uptr<ExecutionStrategyImpl> impl;

public:
  DefaultBuildStrategy(ModuleCollection* coll);
  ~DefaultBuildStrategy();
  void build(Module* module);
  void link(Module* module);
  void run(Module* module);
  bool execute();
};

class GenMakefileStrategy final : public BuildStrategy {
  std::ofstream _output;
  kl::List<kl::Text> _build_targets;
  kl::Set<kl::Text> _build_dirs;

public:
  GenMakefileStrategy(ModuleCollection* coll, kl::Text filename);
  ~GenMakefileStrategy();
  void build(Module* module);
  void link(Module* module);
  void run(Module* module);
  bool execute();
};
