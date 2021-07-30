#pragma once
#include "kltext.h"
#include "klds.h"

namespace kl {

enum class Signal { Interrupt, Terminate, Kill };

class Process {

private:
  struct Impl;
  std::unique_ptr<Impl> _handler;

public:
  Process(const List<Text>& params);
  Process(const Process&) = delete;
  Process(Process&&) = delete;
  ~Process();

  void spawn();
  int  join();

  int run() {
    spawn();
    return join();
  }

  void kill(Signal sig);

  enum class State { NotStarted, Running, Finished, Error };
  State state();
};
} // namespace kl
