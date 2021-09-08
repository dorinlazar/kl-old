#pragma once

#include "modulecollection.h"

struct ExecutionStrategyImpl;

class ExecutionStrategy {
  ModuleCollection* _modules;
  kl::uptr<ExecutionStrategyImpl> impl;

public:
  ExecutionStrategy(ModuleCollection* coll);
  ~ExecutionStrategy();
  void build(Module* module);
  void link(Module* module);
  void run(Module* module);
  bool execute();
};
