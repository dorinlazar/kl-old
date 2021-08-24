#include "poorconfig.h"
using namespace kl;

PValue PoorConfig::parse(const List<Text>& fragment, char split) {
  auto value = Value::createMap();
  for (const auto& line: fragment) {
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
