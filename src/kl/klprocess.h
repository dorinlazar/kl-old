#pragma once
#include "kltext.h"
#include "klds.h"

namespace kl {

enum class Signal { Interrupt, Terminate, Kill };

class Process final {
public:
  Process(const List<Text>& params);
  Process(const Process&) = delete;
  Process(Process&&) = delete;
  ~Process();

  void spawn();
  int join();

  int run() {
    spawn();
    return join();
  }

  void kill(Signal sig);

  enum class State { NotStarted, Running, Finished, Error };
  State state();

  struct Impl;

private:
  uptr<Impl> m_handler;
};

struct ExecutionNode final {
  List<Text> m_params;
  List<ExecutionNode*> m_dependencies;
  Process::State m_state;

public:
  ExecutionNode(const List<Text>& p, const List<ExecutionNode*>& d)
      : m_params(p), m_dependencies(d), m_state(Process::State::NotStarted) {}
};

class ProcessHorde {
  PList<ExecutionNode> m_nodes;
  Queue<ExecutionNode*> m_execution_queue;
  List<ExecutionNode*> m_waiting_queue;

public:
  ExecutionNode* addNode(const List<Text>& params, const List<ExecutionNode*>& deps);
  bool run(uint32_t nJobs, bool verbose = false);
};

} // namespace kl
