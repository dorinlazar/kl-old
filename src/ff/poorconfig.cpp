#include "poorconfig.h"
using namespace kl;

static std::pair<PValue, Text> _readMap(const Text& fragment, char split) {
  auto value = Value::createMap();
  Text nextData = fragment;
  // bool indent = fragment.skipIndent()
  // while (nextData.)
  for (const auto& line: fragment.splitLines()) {
    auto pos = line.pos(split);
    if (pos.has_value()) {
      value->add(line.sublen(0, *pos).trim(), line.subpos((*pos) + 1, line.size()).trim());
    } else {
      auto txt = line.trim();
      if (txt.size() > 0) {
        value->add(line.trim(), ""_t);
      }
    }
  }
  return std::make_pair(value, ""_t);
}

PValue PoorConfig::parse(const Text& fragment, char split) { return _readMap(fragment, split).first; }
