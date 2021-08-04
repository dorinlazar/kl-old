#include "klbtoolchain.h"
#include "klbsettings.h"
#include "kl/klprocess.h"

Gcc::Gcc() {
  _cCompileBase.add({"gcc"_t, "-Wall"_t, "-Wextra"_t, "-c"_t});
  _cCompileBase.add(CMD.cFlags);
  _cppCompileBase.add({"g++"_t, "-Wall"_t, "-Wextra"_t, "-c"_t});
  _cppCompileBase.add(CMD.cxxFlags);
  _linkBase.add({"g++"_t});
  _linkBase.add(CMD.linkFlags);
}

kl::List<kl::Text> Gcc::buildCmdLine(const kl::Text& source, const kl::Text& destination,
                                     const kl::List<kl::Text>& include) {
  kl::List<kl::Text> command = _cppCompileBase;
  command.add("-o"_t);
  command.add(destination);
  command.add(source);
  command.add("-I"_t + CMD.sourceFolder);
  command.add(include.transform<kl::Text>([](const kl::Text& t) { return "-I"_t + t; }));
  return command;
}

kl::List<kl::Text> Gcc::linkCmdLine(const kl::List<kl::Text>& objects, const kl::Text& executable,
                                    const kl::List<kl::Text>& libraries) {
  kl::List<kl::Text> command = _linkBase;
  command.add("-o"_t);
  command.add(executable);
  command.add(objects);
  return command;
}

bool Gcc::build(const kl::Text& source, const kl::Text& destination, const kl::List<kl::Text>& include) {
  if (CMD.verbose) {
    kl::log("Building", destination, "from", source);
  }
  kl::List<kl::Text> command = buildCmdLine(source, destination, include);
  kl::log("> ", kl::TextChain(command).join(' '));
  kl::Process p(command);
  return p.run() == 0;
}

bool Gcc::link(const kl::List<kl::Text>& objects, const kl::Text& executable, const kl::List<kl::Text>& libraries) {
  if (CMD.verbose) {
    kl::log("Linking", executable, "from", objects);
  }
  kl::List<kl::Text> command = linkCmdLine(objects, executable, libraries);

  kl::log("> ", kl::TextChain(command).join(' '));
  kl::Process p(command);
  return p.run() == 0;
}
