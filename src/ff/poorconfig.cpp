#include "poorconfig.h"
#include "textscanner.h"
#include "kl/klfs.h"
using namespace kl;

class PoorConfigParser {
  TextScanner& _scanner;
  char _split;
  const char _comment = '#';
  bool _preamble = false;

  bool _uselessLine() {
    auto startOfLine = _scanner.location();
    auto line = _scanner.read_line().trim_left();
    if (line.size() == 0 || line[0] == _comment) {
      return true;
    }
    _scanner.restoreLocation(startOfLine);
    return false;
  }

  void _discardAfterValueJunk() {
    _scanner.skipWhitespace(NewLineHandling::Keep);
    if (!_scanner.empty()) {
      if (_scanner.topChar() != _comment && _scanner.topChar() != '\n') {
        _scanner.error("Trash at the end of the value");
      }
      _scanner.read_line();
    }
  }

public:
  PoorConfigParser(TextScanner& scanner, char split) : _scanner(scanner), _split(split) {
    if (_scanner.starts_with("---\n"_t)) {
      _scanner.read_line();
      _preamble = true;
    }
  }

  PValue readArray() {
    auto value = Value::createList();
    _scanner.expectws('[');
    bool empty = true;
    while (!_scanner.empty()) {
      _scanner.skipWhitespace();
      if (empty && _scanner.topChar() == ']') {
        _scanner.read_char();
        _discardAfterValueJunk();
        return value;
      }
      value->add(_scanner.readQuotedString());
      _scanner.skipWhitespace();
      auto next = _scanner.read_char();
      if (next.character == ']' && !next.escaped) {
        _discardAfterValueJunk();
        return value;
      }
      if (next.escaped || next.character != ',') {
        _scanner.error("Field separator expected when reading list");
      }
    }
    _scanner.error("Unexpected end of input while scanning list");
    return nullptr;
  }

  PValue readMap(uint32_t minIndent = 0) {
    auto value = Value::createMap();
    std::optional<uint32_t> indentLevel;
    while (!_scanner.empty()) {
      if (_uselessLine()) {
        continue;
      }
      if (_preamble && (_scanner.starts_with("---") || _scanner.starts_with("..."))) {
        if (minIndent == 0) {
          _scanner.skip(3);
          auto loc = _scanner.location();
          if (_scanner.read_line().trim().size() != 0) {
            _scanner.restoreLocation(loc);
          }
        }
        break;
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
        _scanner.read_line();
        value->add(key, readMap(*indentLevel));
      } else if (topChar == '"') {
        Text v = _scanner.readQuotedString();
        _discardAfterValueJunk();
        value->add(key, v);
      } else if (topChar == '[') {
        value->add(key, readArray());
      } else {
        auto [v, comment] = _scanner.read_line().splitNextChar(_comment);
        value->add(key, v.trim());
      }
    }
    return value;
  }
};

PValue PoorConfig::parse(const Text& fragment, char split) {
  TextScanner scanner(fragment);
  PoorConfigParser parser(scanner, split);
  return parser.readMap();
}

PValue PoorConfig::parse(TextScanner& scanner, char split) {
  PoorConfigParser parser(scanner, split);
  return parser.readMap();
}

PoorConfig::PoorConfig(const Text& filename) { _value = parse(FileReader(filename).read_all()); }
PValue PoorConfig::top() const { return _value; }
