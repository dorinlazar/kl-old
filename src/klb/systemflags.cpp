#include "systemflags.h"
#include "kl/klexcept.h"
#include "klbsettings.h"

using namespace kl;

struct Flags {
  List<Text> cxxflags;
  List<Text> ldflags;
  Flags(const Text& cxx, const Text& ld) {
    cxxflags = cxx.splitByChar(' ');
    ldflags = ld.splitByChar(' ');
  }
  Flags(const Text& packageName) {
    for (auto c: packageName) {
      if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '_')) {
        throw InvalidInputData(packageName, "[a-z][A-Z], - or _");
      }
    }
    cxxflags = execute(std::string("pkg-config --cflags ") + packageName.toString()).trim().splitByChar(' ');
    ldflags = execute(std::string("pkg-config --libs ") + packageName.toString()).trim().splitByChar(' ');
  }

private:
  Text execute(const std::string& command) {
    std::array<char, 1024> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) {
      throw IOException::currentStandardError();
    }
    TextChain tc;
    while (!feof(pipe.get())) {
      auto s = fread(buffer.data(), sizeof(char), buffer.size(), pipe.get());
      if (s != 0) {
        tc.add(Text(buffer.data(), s));
      }
    }
    return tc.toText();
  }
};

struct Rule {
  Text rule;
  Flags flags;

  Rule(const Text& r, Flags f) : rule(r), flags(f) {}

  bool applies(const List<Text>& headers) const {
    if (rule.endsWith("/"_t)) {
      for (const auto& h: headers) {
        if (h.startsWith(rule)) {
          return true;
        }
      }
    } else {
      for (const auto& h: headers) {
        if (h == rule) {
          return true;
        }
      }
    }
    return false;
  }
};

struct SystemFlags::SFImpl {

  List<Rule> rules;

  SFImpl(Value* settings) {
    if (settings == nullptr || !settings->isMap()) {
      return;
    }

    for (const auto& [name, description]: settings->asMap()) {
      if (CMD.Verbose()) {
        log("SYSFLAGS: Processing external dependency", name);
      }
      CHECK(description->isMap(), "Expecting external package as map in settings");
      const auto& m = description->asMap();
      auto rule = m["header"]->getValue();
      if (m.has("pkg-config")) {
        rules.add({rule, Flags(m["pkg-config"]->getValue())});
      } else {
        auto cxxf = m.get("cxxflags");
        auto ldf = m.get("ldflags");
        rules.add({rule, Flags(cxxf ? cxxf->getValue() : ""_t, ldf ? ldf->getValue() : ""_t)});
      }
      if (CMD.Verbose()) {
        log("SYSFLAGS: ", name, " cxxflags: ", rules.last().flags.cxxflags);
        log("SYSFLAGS: ", name, "  ldflags: ", rules.last().flags.ldflags);
      }
    }
  }

  List<Text> cxxflags(const List<Text>& headers) {
    List<Text> res;
    for (const auto& r: rules) {
      if (r.applies(headers)) {
        res.add(r.flags.cxxflags);
      }
    }
    return res;
  }
  List<Text> ldflags(const List<Text>& headers) {
    List<Text> res;
    for (const auto& r: rules) {
      if (r.applies(headers)) {
        res.add(r.flags.ldflags);
      }
    }
    return res;
  }
};

SystemFlags::SystemFlags(Value* val) { d = std::make_unique<SFImpl>(val); }
SystemFlags::~SystemFlags() {}
List<Text> SystemFlags::cxxflags(const List<Text>& headers) const { return d->cxxflags(headers); }
List<Text> SystemFlags::ldflags(const List<Text>& headers) const { return d->ldflags(headers); }
