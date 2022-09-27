#include "klprocess.h"
#include "klfs.h"
#include <unistd.h>
#include <sys/wait.h>

using namespace kl;

static char* to_c_string(const Text& t) {
  char* p = (char*)malloc(t.size() + 1);
  std::copy(t.begin(), t.end(), p);
  p[t.size()] = 0;
  return p;
}

static void free_c_string(char* p) { free(p); }

static char** to_c_array(const List<Text>& l) {
  char** res = (char**)malloc(sizeof(char*) * (l.size() + 1));
  char** current = res;
  for (const auto& t: l) {
    *current = to_c_string(t);
    current++;
  }
  *current = nullptr;
  return res;
}

void free_c_array(char** arr) {
  char** current = arr;
  while (*current) {
    free(*current);
    *current = nullptr;
    current++;
  }
  free(arr);
}

struct Process::Impl {
  pid_t _pid = 0;
  bool _started = false;
  char* _exe;
  char** _params;

public:
  Impl(const List<Text>& params) {
    CHECK(params.size() > 0, "Expected more than one parameter to Process invocation");
    _exe = to_c_string(FileSystem::getExecutablePath(params[0]));
    _params = to_c_array(params);
  }

  ~Impl() {
    if (_pid != 0) {
      join();
    }
    free_c_string(_exe);
    free_c_array(_params);
  }

  void spawn() {
    _pid = fork();
    if (_pid == 0) {
      execve(_exe, _params, environ);
      _exit(1);
    }
  }
  int join() {
    if (_pid == 0) {
      return -1;
    }
    int status = 0;
    auto pid = waitpid(_pid, &status, 0);
    if (pid == _pid && WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return -1;
  }

  void kill(Signal sig) {
    if (_pid) {
      int s = 0;
      switch (sig) {
      case Signal::Interrupt: s = SIGINT; break;
      case Signal::Terminate: s = SIGTERM; break;
      case Signal::Kill: s = SIGKILL; break;
      }
      ::kill(_pid, s);
    }
  }

  Process::State state() {
    int status = 0;
    if (!_pid) {
      return Process::State::NotStarted;
    }
    if (waitpid(_pid, &status, WNOHANG)) {
      return Process::State::Error;
    }
    if (WIFEXITED(status)) {
      return Process::State::Finished;
    }
    return Process::State::Running;
  }

  pid_t pid() const { return _pid; }
};

Process::Process(const List<Text>& params) { m_handler = std::make_unique<Impl>(params); }
Process::~Process() {}
void Process::spawn() { m_handler->spawn(); }
int Process::join() { return m_handler->join(); }
void Process::kill(Signal s) { m_handler->kill(s); }

struct ExecutionMonitorNode {
  ExecutionNode* node;
  Process::Impl process;

public:
  ExecutionMonitorNode(ExecutionNode* n) : node(n), process(node->m_params) {}
};

ExecutionNode* ProcessHorde::addNode(const List<Text>& params, const List<ExecutionNode*>& deps) {
  m_nodes.add(std::make_unique<ExecutionNode>(params, deps));
  auto p = m_nodes[m_nodes.size() - 1].get();
  if (deps.size() == 0) {
    m_execution_queue.push(p);
  } else {
    m_waiting_queue.add(p);
  }
  return p;
}

bool ProcessHorde::run(uint32_t nJobs, bool verbose) {
  if (nJobs < 1) {
    nJobs = 1;
  }
  Dict<pid_t, ptr<ExecutionMonitorNode>> monitor;
  while (monitor.size() > 0 || !m_execution_queue.empty()) {
    while (monitor.size() < nJobs && !m_execution_queue.empty()) {
      auto node = m_execution_queue.pop();
      auto monitorNode = std::make_shared<ExecutionMonitorNode>(node);
      if (verbose) {
        kl::log("> [{}]", fmt::join(node->m_params, ","));
      }
      monitorNode->process.spawn();
      monitor.add(monitorNode->process.pid(), std::move(monitorNode));
    }

    int status = 0;
    auto pid = waitpid(-1, &status, 0);
    if (!monitor.has(pid)) {
      continue;
    }

    auto monNode = monitor[pid];
    monitor.remove(pid);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      monNode->node->m_state = Process::State::Finished;

      size_t i = 0;
      while (i < m_waiting_queue.size()) {
        auto task = m_waiting_queue[i];
        if (task->m_dependencies.all(
                [](const kl::ExecutionNode* n) { return n->m_state == Process::State::Finished; })) {
          m_waiting_queue.removeAt(i);
          m_execution_queue.push(task);
        } else {
          i++;
        }
      }
    } else {
      monNode->node->m_state = Process::State::Error;
      return false;
    }
  }

  return true;
}
