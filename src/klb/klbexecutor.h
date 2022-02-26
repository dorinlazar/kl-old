#pragma once

#include "modulecollection.h"

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
  ModuleCollection* _modules;
  kl::uptr<ExecutionStrategyImpl> impl;

public:
  DefaultBuildStrategy(ModuleCollection* coll);
  ~DefaultBuildStrategy();
  void build(Module* module);
  void link(Module* module);
  void run(Module* module);
  bool execute();
};
