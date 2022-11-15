#include "klbtoolchain.h"
#include "klbsettings.h"
#include "kl/klprocess.h"
using namespace kl::literals;

Gcc::Gcc() {}

kl::List<kl::Text> Gcc::buildCmdLine(const kl::Text& source, const kl::Text& destination,
                                     const kl::List<kl::Text>& include, const kl::List<kl::Text>& extraflags) {
  kl::List<kl::Text> command;
  command.add("g++"_t);
  command.add("-c"_t);
  command.add(source);
  command.add("-o"_t);
  command.add(destination);
  command.add("-I"_t + CMD.SourceFolder().full_path());
  command.add(include.transform<kl::Text>([](const kl::Text& t) { return "-I"_t + t; }));
  command.add(extraflags);
  command.add(CMD.CxxFlags());
  command.add("-Wall"_t);
  command.add("-Wextra"_t);
  return command;
}

kl::List<kl::Text> Gcc::linkCmdLine(const kl::List<kl::Text>& objects, const kl::Text& executable,
                                    const kl::List<kl::Text>& ldflags) {
  kl::List<kl::Text> command;
  command.add("g++"_t);
  command.add("-o"_t);
  command.add(executable);
  command.add(objects);
  command.add(CMD.LinkFlags());
  command.add(ldflags);
  return command;
}
