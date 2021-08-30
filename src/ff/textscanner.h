#pragma once
#include <exception>
#include "kl/kltext.h"

namespace kl {
class ParsingError : public std::logic_error {
  uint32_t _line;
  uint32_t _column;

public:
  ParsingError(const Text& why, uint32_t line, uint32_t column);
  uint32_t line() const;
  uint32_t column() const;
};

struct ParsedCharacter {
  char character = '\0';
  bool escaped = false;
};

enum class NewLineHandling { Skip, Keep };

class TextScanner {
  uint32_t _line = 1;
  uint32_t _column = 1;
  uint32_t _offset = 0;

  Text _originalSource;
  const char* _current;
  uint32_t _dataLeft = 0;

  void advance();

public:
  TextScanner(const Text& source);

  void rewind();

  uint32_t line() const;
  uint32_t column() const;

  void skipWhitespace(NewLineHandling handling = NewLineHandling::Skip);
  bool empty() const;
  char topChar() const;
  ParsedCharacter readChar();
  ParsedCharacter readCharEscaped();
  Text readQuotedString();
  Text readWord();
  Text readLine();
  void expectws(char character, NewLineHandling handling = NewLineHandling::Keep);

public:
  void error(const Text& why) const;
};

} // namespace kl
