#include "klbtoolchain.h"
#include "klbsettings.h"
#include "kl/klprocess.h"

Gcc::Gcc() {
  _cCompileBase.add({"gcc"_t, "-c"_t, _cStandard, "-o"_t});
  _cppCompileBase.add({"g++"_t, "-c"_t, _cppStandard, "-o"_t});
  _linkBase.add({"g++"_t, "-o"_t});
}

bool Gcc::build(const kl::Text& source, const kl::Text& destination, const kl::List<kl::Text>& include) {
  if (CMD.verbose) {
    kl::log("Building", destination, "from", source);
  }
  kl::List<kl::Text> command = _cppCompileBase;
  command.add(destination);
  command.add(source);
  command.add("-I"_t + CMD.sourceFolder);
  command.add(include.transform<kl::Text>([](const kl::Text& t) { return "-I"_t + t; }));

  kl::log("> ", kl::TextChain(command).join(' '));
  kl::Process p(command);
  return p.run() == 0;
}

bool Gcc::link(const kl::List<kl::Text>& objects, const kl::Text& executable, const kl::List<kl::Text>& libraries) {
  if (CMD.verbose) {
    kl::log("Linking", executable, "from", objects);
  }
  kl::List<kl::Text> command = _linkBase;
  command.add(executable);
  command.add(objects);

  kl::log("> ", kl::TextChain(command).join(' '));
  kl::Process p(command);
  return p.run() == 0;
}
