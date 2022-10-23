#pragma once

#include "kl/kltext.h"
#include "kl/klds.h"

namespace klblog {

constexpr auto KLBLOG_VERSION{"0.0.1"};

enum class VerbosityLevel { Verbose, Quiet };

struct SystemSettings {
  SystemSettings(int argc, char** argv, char** envp);

  kl::Dict<kl::Text, kl::Text> environment;
  kl::List<kl::Text> arguments;
  kl::Text version = KLBLOG_VERSION;
  VerbosityLevel verbosity = VerbosityLevel::Quiet;
  kl::Text targetFolder = ".";
};

} // namespace klblog
