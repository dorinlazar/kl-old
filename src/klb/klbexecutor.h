#pragma once

#include "modules.h"

struct ExecutionStrategyImpl;

class ExecutionStrategy {
  ModuleCollection* _modules;
  std::unique_ptr<ExecutionStrategyImpl> impl;

public:
  ExecutionStrategy(ModuleCollection* coll);
  ~ExecutionStrategy();
  void build(const kl::Text& module);
  void link(const kl::Text& module);
  bool execute();
};
