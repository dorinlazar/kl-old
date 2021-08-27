#include "poorconfig.h"
using namespace kl;

const Text CommentCharacter = "#"_t;

static std::pair<PValue, Text> _readMap(const Text& fragment, char split, uint32_t minIndent = 0) {
  auto value = Value::createMap();
  Text processingLeftOver = fragment;
  std::optional<uint32_t> indentLevel;
  while (processingLeftOver.size() > 0) {
    auto [currentLine, nextData] = processingLeftOver.splitNextLine();
    if (currentLine.size() > 0 && !currentLine.expectws(CommentCharacter).has_value()) {
      if (!indentLevel.has_value()) {
        indentLevel = currentLine.getIndent();
        if (*indentLevel < minIndent) {
          throw std::invalid_argument("indentation error");
        }
      }
      auto lineContent = currentLine.skipIndent(*indentLevel);
      if (!lineContent.has_value()) {
        return {value, processingLeftOver};
      }
      if (!lineContent->contains(split)) { // we stop processing here
        throw std::invalid_argument("No key - value pair in file");
      }
      auto [name, v] = lineContent->splitNextChar(split);
      auto tname = name.trim();
      auto tvalue = v.trim();
      if (tname.size() == 0) {
        throw std::invalid_argument("Empty key name in configuration file");
      }
      if (tvalue.size() == 0) {
        PValue parsedValue;
        std::tie(parsedValue, nextData) = _readMap(nextData, split, *indentLevel);
        value->add(tname, parsedValue);
      } else {
        value->add(tname, tvalue);
      }
    }
    processingLeftOver = nextData;
  }
  return {value, processingLeftOver};
}

PValue PoorConfig::parse(const Text& fragment, char split) { return _readMap(fragment, split).first; }
