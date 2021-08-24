#pragma once

#include "modules.h"

struct ExecutionStrategyImpl;

class ExecutionStrategy {
  ModuleCollection* _modules;
  kl::uptr<ExecutionStrategyImpl> impl;

public:
  ExecutionStrategy(ModuleCollection* coll);
  ~ExecutionStrategy();
  void build(const kl::Text& module);
  void link(const kl::Text& module);
  bool execute();
};
