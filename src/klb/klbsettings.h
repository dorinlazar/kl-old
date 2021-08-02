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
  kl::Dict<kl::Text, kl::Text> configurationFile;

  void init(int argc, char** argv, char** envp);

private:
  void _updateFlags();
};

extern CommandParameters CMD;
#define VERSION_STRING "0.1"
