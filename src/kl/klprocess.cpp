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
  size_t buffer_size = 0;
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
  pid_t  _pid = 0;
  bool   _started = false;
  char*  _exe;
  char** _params;

public:
  Impl(const List<Text>& params) {
    CHECK(params.size() > 0, "Expected more than one parameter to Process invocation");
    _exe = to_c_string(getExecutablePath(params[0]));
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
    int  status = 0;
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
      case Signal::Interrupt:
        s = SIGINT;
        break;
      case Signal::Terminate:
        s = SIGTERM;
        break;
      case Signal::Kill:
        s = SIGKILL;
        break;
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
};

Process::Process(const List<Text>& params) { _handler = std::make_unique<Impl>(params); }
Process::~Process() {}
void Process::spawn() { _handler->spawn(); }
int  Process::join() { return _handler->join(); }
void Process::kill(Signal s) { _handler->kill(s); }
