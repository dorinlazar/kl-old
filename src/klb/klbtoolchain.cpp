#include "klbtoolchain.h"
#include "klbsettings.h"
#include "kl/klprocess.h"
using namespace kl::literals;

Gcc::Gcc() {
  _cCompileBase.add({"gcc"_t, "-Wall"_t, "-Wextra"_t, "-c"_t});
  _cCompileBase.add(CMD.cFlags);
  _cppCompileBase.add({"g++"_t, "-Wall"_t, "-Wextra"_t, "-c"_t});
  _cppCompileBase.add(CMD.cxxFlags);
  _linkBase.add({"g++"_t});
  _linkBase.add(CMD.linkFlags);
}

kl::List<kl::Text> Gcc::buildCmdLine(const kl::Text& source, const kl::Text& destination,
                                     const kl::List<kl::Text>& include, const kl::List<kl::Text>& extraflags) {
  kl::List<kl::Text> command = _cppCompileBase;
  command.add("-o"_t);
  command.add(destination);
  command.add(source);
  command.add("-I"_t + CMD.sourceFolder.fullPath());
  command.add(include.transform<kl::Text>([](const kl::Text& t) { return "-I"_t + t; }));
  command.add(extraflags);
  return command;
}

kl::List<kl::Text> Gcc::linkCmdLine(const kl::List<kl::Text>& objects, const kl::Text& executable,
                                    const kl::List<kl::Text>& ldflags) {
  kl::List<kl::Text> command = _linkBase;
  command.add("-o"_t);
  command.add(executable);
  command.add(objects);
  command.add(ldflags);
  return command;
}
