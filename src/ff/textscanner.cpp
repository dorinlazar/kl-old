#include "textscanner.h"

using namespace kl;

ParsingError::ParsingError(const Text& why, uint32_t line, uint32_t column)
    : std::logic_error((why + "@"_t + std::to_string(line) + ":" + std::to_string(column)).toText().toString()),
      _line(line), _column(column) {}

uint32_t ParsingError::line() const { return _line; }
uint32_t ParsingError::column() const { return _column; }

TextScanner::TextScanner(const Text& source) : _originalSource(source) {
  loc._current = source.begin();
  loc._dataLeft = source.size();
}

void TextScanner::rewind() {
  loc._line = 1;
  loc._column = 1;
  loc._offset = 0;
  loc._dataLeft = _originalSource.size();
  loc._current = _originalSource.begin();
}

uint32_t TextScanner::line() const { return loc._line; }
uint32_t TextScanner::column() const { return loc._column; }

void TextScanner::skipWhitespace(NewLineHandling handling) {
  Text spaces = handling == NewLineHandling::Skip ? " \t\n\r"_t : " \t"_t;
  while (loc._current < _originalSource.end()) {
    char c = *loc._current;
    if (!spaces.contains(c)) {
      return;
    }
    advance();
  }
}

void TextScanner::advance() {
  if (loc._dataLeft == 0) [[unlikely]] {
    error("Trying to advance beyond end of text");
  }
  if (*loc._current == '\n') {
    loc._line++;
    loc._column = 1;
  } else {
    loc._column++;
  }
  loc._dataLeft--;
  loc._current++;
  loc._offset++;
}

bool TextScanner::empty() const { return loc._dataLeft == 0; }

char TextScanner::topChar() const {
  if (empty()) [[unlikely]] {
    error("Requesting data from empty container");
  }
  return *loc._current;
}

ParsedCharacter TextScanner::readChar() {
  if (empty()) [[unlikely]] {
    error("Reading from empty data");
  }
  ParsedCharacter result{.character = *loc._current};
  advance();
  return result;
}

ParsedCharacter TextScanner::readCharEscaped() {
  if (empty()) [[unlikely]] {
    error("Reading from empty data");
  }
  ParsedCharacter result{.character = *loc._current};
  advance();
  if (result.character == '\\') {
    if (empty()) {
      error("Reading from empty data mid-escape");
    }
    result.escaped = true;
    switch (*loc._current) {
    case 'r': result.character = '\r'; break;
    case 'n': result.character = '\n'; break;
    case '\n': result.character = '\n'; break;
    case 't': result.character = '\t'; break;
    case '0': result.character = '\0'; break;
    case '\\': result.character = '\\'; break;
    case '"': result.character = '"'; break;
    case '\'': result.character = '\''; break;
    default: error("Invalid escape character");
    }
    advance();
  }
  return result;
}

Text TextScanner::readQuotedString() {
  if (empty() || *loc._current != '"') [[unlikely]] {
    error("Unexpected character");
  }
  advance();
  TextChain tc;
  auto _start = loc._offset;
  while (true) {
    auto lastOffset = loc._offset;
    auto ch = readCharEscaped();
    if (ch.escaped) {
      tc.add(_originalSource.subpos(_start, lastOffset - 1));
      tc.add(Text(&ch.character, 1));
      _start = loc._offset;
    } else if (ch.character == '"') {
      tc.add(_originalSource.subpos(_start, lastOffset - 1));
      break;
    }
  }
  return tc.toText();
}

Text TextScanner::readUntil(char character) {
  auto _start = loc._offset;
  while (true) {
    auto ch = readChar();
    if (ch.character == character) {
      return _originalSource.subpos(_start, loc._offset - 2);
    }
  }
}

Text TextScanner::readWord() {
  auto start = loc._offset;
  while (!empty()) {
    char c = *loc._current;
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_' || c == '-')) {
      advance();
    } else {
      break;
    }
  }
  return _originalSource.subpos(start, loc._offset - 1);
}

// TODO make this smarter, to support the \rs as well
Text TextScanner::read_line() {
  auto start = loc._offset;
  while (!empty()) {
    if (*loc._current == '\n') {
      break;
    }
    advance();
  }
  auto res = _originalSource.subpos(start, loc._offset - 1);
  if (!empty()) {
    advance();
  }
  return res;
}

Text TextScanner::remainder() const {
  if (empty()) [[unlikely]] {
    return ""_t;
  }
  return _originalSource.skip(loc._offset);
}

void TextScanner::expect(char character) {
  auto ch = topChar();
  if (ch != character) {
    error("Unexpected character: "_t + Text(ch) + " vs: " + Text(character));
  }
  advance();
}

void TextScanner::expectws(char character, NewLineHandling handling) {
  skipWhitespace(handling);
  expect(character);
}

bool TextScanner::starts_with(const Text& what) { return remainder().starts_with(what); }
void TextScanner::skip(uint32_t nChars) {
  if (nChars < 32) {
    for (uint32_t i = 0; i < nChars; i++) {
      advance();
    }
  } else {
    auto sub = remainder().sublen(0, nChars);
    auto nLines = sub.count('\n');
    loc._offset += sub.size();
    loc._current += sub.size();
    loc._dataLeft -= sub.size();
    if (nLines == 0) {
      loc._column += sub.size();
    } else {
      loc._line += nLines;
      loc._column = 1 + (sub.size() - *sub.pos('\n', nLines));
    }
  }
}

void TextScanner::error(const Text& why) const { throw ParsingError(why, loc._line, loc._column); }

const TextScanner::DataLocation& TextScanner::location() const { return loc; }
void TextScanner::restoreLocation(const TextScanner::DataLocation& location) { loc = location; }

uint32_t TextScanner::getIndentationLevel() const {
  for (uint32_t i = 0; i < loc._dataLeft; i++) {
    if (loc._current[i] != ' ') {
      return i;
    }
  }
  return loc._dataLeft;
}

uint32_t TextScanner::readDigit() {
  if (empty()) [[unlikely]] {
    error("Reading from empty data");
  }
  char c = *loc._current;
  if (c < '0' || c > '9') [[unlikely]] {
    error("Expected a digit");
  }
  advance();
  return c - '0';
}
