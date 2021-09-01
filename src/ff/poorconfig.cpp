#include "poorconfig.h"
#include "textscanner.h"
using namespace kl;

const Text CommentCharacter = "#"_t;

class PoorConfigParser {
  TextScanner _scanner;
  char _split;
  const char _comment = '#';

  bool _uselessLine() {
    auto startOfLine = _scanner.location();
    auto line = _scanner.readLine();
    if (line.trim().size() == 0 || line.expectws(CommentCharacter).has_value()) {
      return true;
    }
    _scanner.restoreLocation(startOfLine);
    return false;
  }

public:
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
      if (_uselessLine()) {
        continue;
      }
      if (!indentLevel.has_value()) {
        indentLevel = _scanner.getIndentationLevel();
        if (*indentLevel < minIndent) {
          _scanner.error("Invalid indentation for start of map");
        }
      }
      auto currentIndent = _scanner.getIndentationLevel();
      if (indentLevel > currentIndent) { // end for this level of indentation
        return value;
      }
      if (indentLevel < _scanner.getIndentationLevel()) {
        _scanner.error("Bad indentation for map key/value pair");
      }
      _scanner.skipWhitespace(NewLineHandling::Keep);
      auto key = _scanner.readWord();
      if (key.size() == 0) {
        _scanner.error("Empty key or invalid indentation");
      }
      _scanner.expectws(_split);
      _scanner.skipWhitespace(NewLineHandling::Keep);
      char topChar = _scanner.topChar();
      if (topChar == '\n' || topChar == _comment) { // we'll have a map value
        _scanner.readLine();
        value->add(key, readMap(*indentLevel));
      } else if (topChar == '"') {
        Text v = _scanner.readQuotedString();
        _scanner.skipWhitespace(NewLineHandling::Keep);
        if (!_scanner.empty() && _scanner.topChar() != _comment && _scanner.topChar() != '\n') {
          _scanner.error("Text after value");
        }
        value->add(key, v);
      } else if (topChar == '[') {
        value->add(key, readArray());
      } else {
        auto [v, comment] = _scanner.readLine().splitNextChar(_comment);
        value->add(key, v);
      }
    }
    return value;
  }
};

PValue PoorConfig::parse(const Text& fragment, char split) {
  PoorConfigParser parser(fragment, split);
  return parser.readMap();
  // return _readMap(fragment, split).first;
}
