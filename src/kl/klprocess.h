#pragma once
#include "kltext.h"
#include "klds.h"

namespace kl {

enum class Signal { Interrupt, Terminate, Kill };

class Process {
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
  std::unique_ptr<Impl> _handler;
};

struct ExecutionNode {
  List<Text> params;
  List<ExecutionNode*> dependencies;
  Process::State state;

public:
  ExecutionNode(const List<Text>& p, const List<ExecutionNode*>& d)
      : params(p), dependencies(d), state(Process::State::NotStarted) {}
};

class ProcessHorde {
  List<std::unique_ptr<ExecutionNode>> _nodes;
  Queue<ExecutionNode*> _executionQueue;
  List<ExecutionNode*> _waitingQueue;

public:
  ExecutionNode* addNode(const List<Text>& params, const List<ExecutionNode*>& deps);
  bool run(uint32_t nJobs);
};

} // namespace kl
