#include "textscanner.h"

using namespace kl;

ParsingError::ParsingError(const Text& why, uint32_t line, uint32_t column)
    : std::logic_error(why.toString()), _line(line), _column(column) {}

uint32_t ParsingError::line() const { return _line; }
uint32_t ParsingError::column() const { return _column; }

TextScanner::TextScanner(const Text& source)
    : _originalSource(source), _current(source.begin()), _dataLeft(source.size()) {}

void TextScanner::rewind() {
  _current = _originalSource.begin();
  _dataLeft = _originalSource.size();
  _line = 1;
  _column = 1;
}
uint32_t TextScanner::line() const { return _line; }
uint32_t TextScanner::column() const { return _column; }

void TextScanner::skipWhitespace(NewLineHandling handling) {
  Text spaces = handling == NewLineHandling::Skip ? " \t\n\r"_t : " \t"_t;
  while (_current < _originalSource.end()) {
    char c = *_current;
    if (!spaces.contains(c)) {
      return;
    }
    advance();
  }
}

void TextScanner::advance() {
  _offset++;
  if (*_current == '\n') {
    _line++;
    _column = 1;
  } else {
    _column++;
  }
  _current++;
  _dataLeft--;
}

bool TextScanner::empty() const { return _dataLeft == 0; }

char TextScanner::topChar() const {
  if (empty()) {
    throw ParsingError("Requesting data from empty container", _line, _column);
  }
  return *_current;
}

ParsedCharacter TextScanner::readChar() {
  if (empty()) {
    throw ParsingError("Reading from empty data"_t, _line, _column);
  }
  ParsedCharacter result{.character = *_current};
  advance();
  return result;
}

ParsedCharacter TextScanner::readCharEscaped() {
  if (empty()) {
    throw ParsingError("Reading from empty data"_t, _line, _column);
  }
  ParsedCharacter result{.character = *_current};
  advance();
  if (result.character == '\\') {
    if (empty()) {
      throw ParsingError("Reading from empty data mid-escape"_t, _line, _column);
    }
    result.escaped = true;
    switch (*_current) {
    case 'r': result.character = '\r'; break;
    case 'n': result.character = '\n'; break;
    case 't': result.character = '\t'; break;
    case '0': result.character = '\0'; break;
    case '\\': result.character = '\\'; break;
    case '"': result.character = '"'; break;
    case '\'': result.character = '\''; break;
    default: throw ParsingError("Invalid escape character"_t, _line, _column);
    }
    advance();
  }
  return result;
}

Text TextScanner::readQuotedString() {
  if (empty() || *_current != '"') {
    throw ParsingError("Unexpected character", _line, _column);
  }
  advance();
  TextChain tc;
  auto _start = _offset;
  while (true) {
    auto lastOffset = _offset;
    auto ch = readCharEscaped();
    if (ch.escaped) {
      tc.add(_originalSource.subpos(_start, lastOffset - 1));
      tc.add(Text(&ch.character, 1));
      _start = _offset;
    } else if (ch.character == '"') {
      tc.add(_originalSource.subpos(_start, lastOffset - 1));
      break;
    }
  }
  return tc.toText();
}
