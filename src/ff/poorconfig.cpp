#include "poorconfig.h"
#include "textscanner.h"
using namespace kl;

const Text CommentCharacter = "#"_t;

struct PoorConfigParser {
  TextScanner _scanner;
  char _split;
  const char _comment = '#';

  PoorConfigParser(const Text& fragment, char split) : _scanner(fragment), _split(split) {}

  PValue readArray() {
    auto value = Value::createList();
    // TODO magic!!!
    return value;
  }

  PValue readMap(uint32_t minIndent = 0) {
    auto value = Value::createMap();
    std::optional<uint32_t> indentLevel;
    while (!_scanner.empty()) {
      auto startOfLine = _scanner.location();
      auto line = _scanner.readLine();
      if (line.size() == 0 || line.expectws(CommentCharacter).has_value()) {
        continue;
      }
      if (!indentLevel.has_value()) {
        indentLevel = line.getIndent();
        if (*indentLevel < minIndent) {
          _scanner.error("Invalid indentation for start of map");
        }
      }
      auto lineContent = line.skipIndent(*indentLevel);
      // test if we have less indentation than the currently set indentation level,
      // so we need to go to the higher level
      if (!lineContent.has_value()) {
        _scanner.restoreLocation(startOfLine);
        return value;
      }

      TextScanner lineScanner(*lineContent);
      auto key = lineScanner.readWord();
      if (key.size() == 0) {
        _scanner.error("Empty key or invalid indentation");
      }
      lineScanner.expectws(_split);
      lineScanner.skipWhitespace();
      if (lineScanner.empty() || lineScanner.topChar() == _comment) { // we'll have a map value
        value->add(key, readMap(*indentLevel));
      } else {
        if (lineScanner.topChar() == '"') {
          Text v = lineScanner.readQuotedString();
          lineScanner.skipWhitespace();
          if (!lineScanner.empty() && lineScanner.topChar() != _comment) {
            throw ParsingError("Text after value", _scanner.line() - 1, lineScanner.column() + *indentLevel);
          }
          value->add(key, v);
        } else if (lineScanner.topChar() == '[') {
          value->add(key, readArray());
        } else {
          auto [v, comment] = lineScanner.remainder().splitNextChar(_comment);
          value->add(key, v);
        }
      }
    }
    return value;
  }
};

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

PValue PoorConfig::parse(const Text& fragment, char split) {
  PoorConfigParser parser(fragment, split);
  return parser.readMap();
  // return _readMap(fragment, split).first;
}
