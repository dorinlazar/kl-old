#pragma once
#include "kl.h"
#include "klds.h"
#include <algorithm> // this comment is a test, really
#include <compare>
#include <cstring>
#include <memory>
#include <optional>
#include <ostream>
#include <string_view>
#include <vector>
#include <span>
#include <fmt/format.h>

namespace kl {

enum class SplitEmpty { Keep, Discard };
enum class SplitDirection { KeepLeft, Discard, KeepRight };

class TextView {
public:
  TextView() = default;
  TextView(std::string_view v);
  TextView(const char* text);
  TextView(const char* text, size_t length);
  TextView(const TextView&) = default;
  TextView(TextView&&) = default;
  TextView& operator=(const TextView&) = default;
  TextView& operator=(TextView&&) = default;
  ~TextView() = default;

public:
  TextView trim() const;
  TextView trimLeft() const;
  TextView trimRight() const;

  bool startsWith(char c) const;
  bool startsWith(const TextView& tv) const;
  bool endsWith(char c) const;
  bool endsWith(const TextView& tv) const;

  char operator[](size_t index) const;

  size_t size() const;
  const char* begin() const;
  const char* end() const;

  std::strong_ordering operator<=>(const TextView&) const;
  std::strong_ordering operator<=>(const std::string_view&) const;
  std::strong_ordering operator<=>(const std::string&) const;
  std::strong_ordering operator<=>(const char*) const;
  bool operator==(const TextView&) const;
  bool operator==(const std::string_view&) const;
  bool operator==(const std::string&) const;
  bool operator==(const char*) const;

  operator std::string_view() const;
  std::string_view view() const;

  bool contains(char c) const;

  TextView skip(const TextView& skippables) const;
  TextView skip(size_t n) const;
  TextView skipBOM() const;

  // substring position based. The string will contain the character from ending position too.
  TextView subpos(size_t start, size_t end) const;

  // substring length based. The return value will have a string of at most <len> characters
  TextView sublen(size_t start, size_t len) const;

  // occurence is one based - so first occurence is 1;
  std::optional<size_t> pos(char c, size_t occurence = 1) const;
  std::optional<size_t> pos(const TextView& t, size_t occurence = 1) const;
  std::optional<size_t> lastPos(char c) const;

  std::pair<TextView, TextView> splitPos(ssize_t where) const;
  std::pair<TextView, TextView> splitNextChar(char c, SplitDirection direction = SplitDirection::Discard) const;
  std::pair<TextView, TextView> splitNextLine() const;
  List<TextView> splitLines(SplitEmpty onEmpty = SplitEmpty::Keep) const;
  List<TextView> splitByChar(char c, SplitEmpty onEmpty = SplitEmpty::Discard) const;
  List<TextView> splitByText(const TextView& t, SplitEmpty onEmpty = SplitEmpty::Discard) const;

  // returns a value that skips the starting text
  std::optional<TextView> expect(const TextView& t) const;
  // returns a value that skips the whitespace in the text
  std::optional<TextView> expectws(const TextView& t) const;

  // returns text after <indentLevel> whitespaces, or empty;
  std::optional<TextView> skipIndent(size_t indentLevel) const;
  // returns whitespace indent level
  size_t getIndent() const;
  // how many times the character c appears in the text
  size_t count(char c) const;

private:
  std::string_view m_view;
};

class TextRefCounter {
  int64_t ref_count = 1;
  char block_start[0];

public:
  TextRefCounter* acquire();
  bool release();
  char* text_data();
  static TextRefCounter* allocate(size_t text_size);
  static TextRefCounter s_empty;
};
static_assert(sizeof(TextRefCounter) == sizeof(int64_t));

class Text {
  TextRefCounter* m_memblock;
  size_t m_start = 0;
  size_t m_end = 0;

public:
  Text();
  ~Text();
  Text(const Text&);
  Text(Text&&);
  Text& operator=(const Text& v);
  Text& operator=(Text&& v);

  Text(char c);
  Text(const std::string& s);
  Text(const char* ptr);
  Text(const char* ptr, size_t size);
  Text(const Text& t, size_t start, size_t length);
  Text(TextRefCounter* buffer, size_t length);

public:
  void reset();
  // This is useful when we don't want to keep the big source text like when we're parsing some file
  // for a small bit of information and we want to free that piece of memory.
  Text copy() const;

  char operator[](ssize_t index) const;

  std::strong_ordering operator<=>(const Text&) const;
  std::strong_ordering operator<=>(const char*) const;
  std::strong_ordering operator<=>(const std::string&) const;

  bool operator==(const Text&) const;
  bool operator==(const char*) const;
  bool operator==(const std::string&) const;

public:
  size_t size() const;
  const char* begin() const;
  const char* end() const;

  Text trim() const;
  Text trimLeft() const;
  Text trimRight() const;

  bool startsWith(const Text& tv) const;
  bool startsWith(const char*) const;
  bool startsWith(char c) const;
  bool endsWith(const Text& tv) const;
  bool endsWith(char c) const;

  std::string toString() const;
  std::string_view toView() const;
  TextView toTextView() const;
  std::span<uint8_t> toRawData() const;
  int64_t toInt() const;

  bool contains(char c) const;
  Text skip(std::string_view skippables) const;
  Text skip(size_t n) const;
  Text skipBOM() const;

  // substring position based. The string will contain the character from ending position too.
  Text subpos(size_t start, size_t end) const;

  // substring length based. The return value will have a string of at most <len> characters
  Text sublen(size_t start, size_t len) const;

  // occurence is one based - so first occurence is 1;
  std::optional<size_t> pos(char c, size_t occurence = 1) const;
  std::optional<size_t> pos(Text t, size_t occurence = 1) const;
  std::optional<size_t> lastPos(char c) const;

  std::pair<Text, Text> splitPos(ssize_t where) const;
  std::pair<Text, Text> splitNextChar(char c, SplitDirection direction = SplitDirection::Discard) const;
  std::pair<Text, Text> splitNextLine() const;
  List<Text> splitLines(SplitEmpty onEmpty = SplitEmpty::Keep) const;
  List<Text> splitByChar(char c, SplitEmpty onEmpty = SplitEmpty::Discard) const;
  List<Text> splitByText(const Text& t, SplitEmpty onEmpty = SplitEmpty::Discard) const;

  // returns a value that skips the starting text
  std::optional<Text> expect(const Text& t) const;
  // returns a value that skips the whitespace in the text
  std::optional<Text> expectws(const Text& t) const;

  // returns text after <indentLevel> whitespaces, or empty;
  std::optional<Text> skipIndent(size_t indentLevel) const;
  // returns whitespace indent level
  size_t getIndent() const;

  // fills a C buffer, preallocated with bufsize bytes;
  void fill_c_buffer(char* dest, size_t bufsize) const;
  // how many times the character c appears in the text
  size_t count(char c) const;
};

class TextChain {
  uint32_t _length = 0;
  List<Text> _chain;

  void _updateLength();

public:
  TextChain() = default;
  TextChain(const TextChain&) = default;
  TextChain(TextChain&&) = default;
  TextChain(std::initializer_list<Text> l);
  TextChain(List<Text>&& l);
  TextChain(const List<Text>& l);

  inline TextChain& operator=(const TextChain& v) = default;

  void operator+=(const Text& text);
  void add(const Text& text);
  void operator+=(const TextChain& text);
  void add(const TextChain& text);

  Text toText() const;
  operator Text() const;

  const List<Text>& chain() const;

  void clear();
  kl::Text join(char splitchar = '\0');
};

inline namespace literals {
kl::Text operator"" _t(const char* p, size_t s);
}

std::ostream& operator<<(std::ostream& os, const TextView& tv);
std::ostream& operator<<(std::ostream& os, const Text& tv);
std::ostream& operator<<(std::ostream& os, const TextChain& tv);

} // namespace kl

template <>
struct std::hash<kl::Text> {
  std::size_t operator()(const kl::Text& s) const noexcept;
};

template <>
struct fmt::formatter<kl::Text> : public fmt::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(kl::Text c, FormatContext& ctx) const {
    return fmt::formatter<std::string_view>::format(c.toView(), ctx);
  }
};

kl::TextChain operator+(const kl::Text&, const char*);
kl::TextChain operator+(const kl::Text&, const kl::Text&);
kl::TextChain operator+(const kl::TextChain&, const kl::Text&);
kl::TextChain operator+(const kl::TextChain&, const char*);
