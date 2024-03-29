#pragma once

#include "kl/kltext.h"
#include "kl/klds.h"

namespace klblog {

constexpr auto KLBLOG_VERSION{"0.0.1"};

enum class VerbosityLevel { Verbose, Quiet };

struct SystemSettings {
  SystemSettings(int argc, char** argv, char** envp);

  bool verbose() const;

  kl::Dict<kl::Text, kl::Text> environment;
  kl::List<kl::Text> arguments;
  kl::Text version = KLBLOG_VERSION;
  VerbosityLevel verbosity = VerbosityLevel::Quiet;
  kl::Text source_folder = ".";
  kl::Text destination_folder = "blog/";
};

} // namespace klblog
