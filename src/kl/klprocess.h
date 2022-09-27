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
  uptr<Impl> _handler;
};

struct ExecutionNode final {
  List<Text> params;
  List<ExecutionNode*> dependencies;
  Process::State state;

public:
  ExecutionNode(const List<Text>& p, const List<ExecutionNode*>& d)
      : params(p), dependencies(d), state(Process::State::NotStarted) {}
};

class ProcessHorde {
  PList<ExecutionNode> _nodes;
  Queue<ExecutionNode*> _executionQueue;
  List<ExecutionNode*> _waitingQueue;

public:
  ExecutionNode* addNode(const List<Text>& params, const List<ExecutionNode*>& deps);
  bool run(uint32_t nJobs, bool verbose = false);
};

} // namespace kl
