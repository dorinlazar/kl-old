#pragma once

#pragma once

#include "modulecollection.h"

class GenMakefileExecutionStrategy {
  ModuleCollection* _modules;

public:
  GenMakefileExecutionStrategy(ModuleCollection* coll);
  ~GenMakefileExecutionStrategy();
  void build(Module* module);
  void link(Module* module);
  void run(Module* module);
  bool execute();
};
