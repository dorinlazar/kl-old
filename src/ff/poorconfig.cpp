#include "poorconfig.h"
using namespace kl;

// static std::tuple<PValue, Text> _readMap(const Text& fragment, char split) {
//   auto value = Value::createMap();
//   return std::make_tuple(value, fragment);
// }

PValue PoorConfig::parse(const Text& fragment, char split) {
  auto value = Value::createMap();
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
  return value;
}
