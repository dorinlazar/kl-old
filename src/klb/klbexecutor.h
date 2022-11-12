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
  ~DefaultBuildStrategy() override;
  void build(Module* module) override;
  void link(Module* module) override;
  void run(Module* module) override;
  bool execute() override;
};

class GenMakefileStrategy final : public BuildStrategy {
  std::ofstream m_makefile_output;
  std::ofstream m_compilation_db_output;
  kl::List<kl::Text> _build_targets;
  kl::Set<kl::Text> _build_dirs;
  std::map<kl::Text, kl::Set<kl::Text>> _extra_targets;
  bool m_first_operation = true;

public:
  GenMakefileStrategy(ModuleCollection* coll, kl::Text makefile_filename, kl::Text compilation_db_filename);
  ~GenMakefileStrategy() override;
  void build(Module* module) override;
  void link(Module* module) override;
  void run(Module* module) override;
  bool execute() override;
};
