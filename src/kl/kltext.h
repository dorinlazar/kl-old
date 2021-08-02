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

namespace kl {

enum class SplitEmpty { Keep, Discard };

class Text {
  // the only place where we unconst this is in constructors. replace void with const char
  static std::shared_ptr<char> s_null_data;
  std::shared_ptr<char> _memblock = s_null_data;
  uint32_t _start = 0;
  uint32_t _end = 0;

public:
  Text() = default;
  ~Text() = default;
  Text(const Text&) = default;
  Text(Text&&) = default;

  Text(const std::string& s) {
    _end = s.size();
    if (_end > 0) {
      // TODO replace as soon as gcc implements with make_shared_for_overwrite
      _memblock = std::shared_ptr<char>((char*)malloc(_end), free);
      std::copy(s.begin(), s.end(), _memblock.get());
    }
  }

  Text(const char* ptr) {
    if (ptr) {
      _end = std::strlen(ptr);
      if (_end > 0) {
        // TODO replace as soon as gcc implements with make_shared_for_overwrite
        _memblock = std::shared_ptr<char>((char*)malloc(_end), free);
        std::copy(ptr, ptr + _end, _memblock.get());
      }
    }
  }

  Text(const char* ptr, uint32_t max) {
    if (ptr) {
      _end = std::min((uint32_t)std::strlen(ptr), max);
      if (_end > 0) {
        // TODO replace as soon as gcc implements with make_shared_for_overwrite
        _memblock = std::shared_ptr<char>((char*)malloc(_end), free);
        std::copy(ptr, ptr + _end, _memblock.get());
      }
    }
  }

  Text(const Text& t, uint32_t start, uint32_t length) {
    _memblock = t._memblock;
    _start = std::min(t._start + start, t._end);
    _end = std::min(_start + length, t._end);
    if (_start == _end) {
      _memblock = s_null_data;
      _start = 0;
      _end = 0;
    }
  }

  Text& operator=(const Text& v) {
    _memblock = v._memblock;
    _start = v._start;
    _end = v._end;
    return *this;
  }

  static Text FromBuffer(std::shared_ptr<char> p, uint32_t start, uint32_t end) {
    if (start >= end || p.get() == nullptr) {
      return {};
    }
    Text t;
    t._memblock = p;
    t._start = start;
    t._end = end;
    return t;
  }

public:
  Text trim() const;
  Text trimLeft() const;
  Text trimRight() const;

  // This is useful when we don't want to keep the big source text like when we're parsing some file
  // for a small bit of information and we want to free that piece of memory.
  Text copy() const { return Text(_memblock.get() + _start, size()); }

  bool startsWith(const Text& tv) const;
  bool startsWith(const char*) const;
  bool endsWith(const Text& tv) const;

  char operator[](uint32_t index) const {
    CHECKST(index < size()); // This is brutal. Do I really need this? Alternative: return \0
    return *(_memblock.get() + _start + index);
  }

  uint32_t size() const { return _end - _start; }
  const char* begin() const { return _memblock.get() ? _memblock.get() + _start : nullptr; }
  const char* end() const { return _memblock.get() + _end; }

  std::strong_ordering operator<=>(const Text&) const;
  std::strong_ordering operator<=>(const char*) const;
  std::strong_ordering operator<=>(const std::string&) const;

  bool operator==(const Text&) const;
  bool operator==(const char*) const;
  bool operator==(const std::string&) const;

  std::string toString() const { return std::string((const char*)_memblock.get() + _start, size()); }
  std::string_view toView() const { return std::string_view(_memblock.get() + _start, (size_t)size()); }
  int64_t toInt() const;

  bool contains(char c) const;
  Text skip(const Text& skippables) const;
  Text skip(uint32_t n) const;

  // substring position based. The string will contain the character from ending position too.
  Text subpos(uint32_t start, uint32_t end) const {
    if (start >= size() || end < start) {
      return Text();
    }
    end++; // include the ending character
    if (end > size()) {
      end = size();
    }
    return Text::FromBuffer(_memblock, _start + start, _start + end);
  }

  // substring length based. The return value will have a string of at most <len> characters
  Text sublen(uint32_t start, uint32_t len) const {
    if (start >= size()) {
      return Text();
    }
    if (start + len > size()) {
      len = size() - start;
    }
    return Text::FromBuffer(_memblock, _start + start, _start + start + len);
  }

  // occurence is one based - so first occurence is 1;
  std::optional<uint32_t> pos(char c, uint32_t occurence = 1) const;
  std::optional<uint32_t> pos(Text t, uint32_t occurence = 1) const;
  std::optional<uint32_t> lastPos(char c) const;

  std::tuple<Text, Text> splitPos(int32_t where) const;
  List<Text> splitLines(SplitEmpty onEmpty = SplitEmpty::Keep) const;
  List<Text> splitByChar(char c, SplitEmpty onEmpty = SplitEmpty::Discard) const;
  List<Text> splitByText(const Text& t, SplitEmpty onEmpty = SplitEmpty::Discard) const;

  std::optional<Text> expect(const Text& t) const { // returns a value that skips the starting text
    if (startsWith(t)) {
      return skip(t.size());
    }
    return {};
  }
  std::optional<Text> expectws(const Text& t) { // returns a value that skips the starting text, skipping the whitespace
    return trimLeft().expect(t);
  }
  void fill_c_buffer(char* dest, uint32_t bufsize) const {
    if (bufsize == 0) {
      return;
    }
    uint32_t amount_to_copy = bufsize <= size() ? bufsize - 1 : size();
    std::copy(begin(), begin() + amount_to_copy, dest);
    dest[amount_to_copy] = 0;
  }

  uint32_t count(char t) const {
    uint32_t count = 0;
    for (auto c: *this) {
      if (c == t) {
        count++;
      }
    }
    return count;
  }
};

class TextChain {
  uint32_t _length = 0;
  List<Text> _chain;

  void _updateLength() {
    _length = 0;
    for (const auto& t: _chain) {
      _length += t.size();
    }
  }

public:
  TextChain() = default;
  TextChain(const TextChain&) = default;
  TextChain(TextChain&&) = default;
  TextChain(std::initializer_list<Text> l) : _chain(l) { _updateLength(); }
  TextChain(List<Text>&& l) : _chain(l) { _updateLength(); }
  TextChain(const List<Text>& l) : _chain(l) { _updateLength(); }

  const List<Text>& chain() const { return _chain; }

  inline TextChain& operator=(const TextChain& v) = default;

  void operator+=(const Text& text) {
    _chain.add(text);
    _length += text.size();
  }

  void add(const Text& text) {
    _chain.add(text);
    _length += text.size();
  };

  void operator+=(const TextChain& text) {
    _chain.add(text._chain);
    _length += text._length;
  }

  void add(const TextChain& text) {
    _chain.add(text._chain);
    _length += text._length;
  };

  Text toText() const;

  operator Text() const { return toText(); }

  void clear() {
    _chain = {};
    _length = 0;
  }

  void consolidate();
  kl::Text join(char splitchar = '\0');
};

} // namespace kl

static inline kl::Text operator"" _t(const char* p, size_t s) { return kl::Text(p, s); }

namespace std {

template <>
struct hash<kl::Text> {
  std::size_t operator()(const kl::Text& s) const noexcept { return std::hash<std::string_view>{}(s.toView()); }
};

ostream& operator<<(ostream& os, const kl::Text& t);

} // namespace std

kl::TextChain operator+(const kl::Text&, const char*);
kl::TextChain operator+(const kl::Text&, const kl::Text&);
kl::TextChain operator+(const kl::TextChain&, const kl::Text&);
kl::TextChain operator+(const kl::TextChain&, const char*);
