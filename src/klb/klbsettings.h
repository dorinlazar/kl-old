#pragma once
#include "kl/kltext.h"

struct CommandParameters {
  bool verbose = false;
  kl::Text buildFolder = "build"_t;
  kl::Text sourceFolder = "src"_t;
};

extern CommandParameters CMD;
#define VERSION_STRING "0.1"
