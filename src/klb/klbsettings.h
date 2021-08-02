#pragma once
#include "kl/kltext.h"

struct CommandParameters {
  bool verbose = false;
  kl::Text buildFolder = "build"_t;
  kl::Text sourceFolder = "src"_t;
  kl::List<kl::Text> cxxFlags;
  kl::List<kl::Text> cFlags;
  kl::List<kl::Text> linkFlags;
  kl::Dict<kl::Text, kl::Text> environment;

  void init(int argc, char** argv, char** envp);
};

extern CommandParameters CMD;
#define VERSION_STRING "0.1"
