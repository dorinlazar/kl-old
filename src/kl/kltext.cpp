#include "kltext.h"
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <numeric>

using namespace std::literals;

namespace kl {

constexpr std::string_view WHITESPACE = " \t\n\r"sv;

TextView::TextView(std::string_view v) : m_view(v) {}
TextView::TextView(const char* text) : m_view(text) {}
TextView::TextView(const char* text, size_t length) : m_view(text, length) {}

TextView TextView::trim() const { return trim_left().trim_right(); }
TextView TextView::trim_left() const { return skip(WHITESPACE); }
TextView TextView::trim_right() const {
  auto position = m_view.find_last_not_of(WHITESPACE);
  if (position == std::string_view::npos) {
    return {};
  }
  return m_view.substr(0, position + 1);
}

bool TextView::startsWith(char c) const { return m_view.size() > 0 && m_view.front() == c; }
bool TextView::startsWith(const TextView& tv) const { return m_view.starts_with(tv.m_view); }
bool TextView::endsWith(char c) const { return m_view.size() > 0 && m_view.back() == c; }
bool TextView::endsWith(const TextView& tv) const { return m_view.ends_with(tv.m_view); }

char TextView::operator[](size_t index) const {
  if (index >= size()) [[unlikely]] {
    throw std::out_of_range(fmt::format("Requested index {} in a {} long TextView", index, size()));
  }
  return m_view[index];
}

size_t TextView::size() const { return m_view.size(); }
const char* TextView::begin() const { return m_view.begin(); }
const char* TextView::end() const { return m_view.end(); }

std::strong_ordering TextView::operator<=>(const TextView& v) const { return m_view <=> v.m_view; }
std::strong_ordering TextView::operator<=>(const std::string_view& sv) const { return m_view <=> sv; }
std::strong_ordering TextView::operator<=>(const std::string& s) const { return m_view <=> s; }
std::strong_ordering TextView::operator<=>(const char* s) const { return m_view <=> s; }
bool TextView::operator==(const TextView& v) const { return m_view == v.m_view; }
bool TextView::operator==(const std::string_view& sv) const { return m_view == sv; }
bool TextView::operator==(const std::string& s) const { return m_view == s; }
bool TextView::operator==(const char* s) const { return m_view == s; }

TextView::operator std::string_view() const { return m_view; }
std::string_view TextView::view() const { return m_view; }

bool TextView::contains(char c) const { return pos(c).has_value(); }

TextView TextView::skip(const TextView& skippables) const {
  auto v = m_view;
  v.remove_prefix(std::min(v.find_first_not_of(skippables.m_view), v.size()));
  return v;
}
TextView TextView::skip(size_t size) const { return m_view.substr(size); }
TextView TextView::skipBOM() const {
  if (size() >= 3) {
    auto buf = begin();
    if (buf[0] == (char)0xEF && buf[1] == (char)0xBB && buf[2] == (char)0xBF) {
      return skip(3);
    }
  }
  return *this;
}

// substring position based. The string will contain the character from ending position too.
TextView TextView::subpos(size_t start, size_t end) const {
  if (start > end) {
    return {};
  }

  return m_view.substr(std::min(start, size()), 1 + (std::min(end, size()) - start));
}

// substring length based. The return value will have a string of at most <len> characters
TextView TextView::sublen(size_t start, size_t len) const { return m_view.substr(start, len); }

std::optional<size_t> TextView::pos(char c, size_t occurence) const {
  if (occurence == 0) {
    return std::nullopt;
  }
  size_t position = 0;
  while (position < m_view.size()) {
    position = m_view.find_first_of(c, position);
    if (position != std::string_view::npos) {
      occurence--;
      if (occurence == 0) {
        return position;
      }
      position++;
    }
  }

  return std::nullopt;
}

std::optional<size_t> TextView::pos(const TextView& t, size_t occurence) const {
  if (occurence == 0 || t.size() == 0) {
    return std::nullopt;
  }
  size_t position = 0;
  while (position < m_view.size()) {
    position = m_view.find(t.m_view, position);
    if (position < m_view.size()) {
      occurence--;
      if (occurence == 0) {
        return position;
      }
      position += t.size();
    }
  }

  return std::nullopt;
}

std::optional<size_t> TextView::lastPos(char c) const {
  auto res = m_view.find_last_of(c);
  if (res != std::string_view::npos) {
    return res;
  }
  return std::nullopt;
}

std::pair<TextView, TextView> TextView::splitPos(ssize_t where) const {
  size_t pos = where > 0 ? std::min(size(), static_cast<size_t>(where))
                         : (size() - std::min(static_cast<size_t>(-where), size()));
  return {m_view.substr(0, pos), m_view.substr(pos)};
}

std::pair<TextView, TextView> TextView::splitNextChar(char c, SplitDirection direction) const {
  auto pos = m_view.find_first_of(c);
  if (pos == std::string_view::npos) {
    return {m_view, {}};
  }
  if (direction == SplitDirection::Discard) {
    return {m_view.substr(0, pos), m_view.substr(pos + 1)};
  }
  if (direction == SplitDirection::KeepLeft) {
    pos++;
  }
  return {m_view.substr(0, pos), m_view.substr(pos)};
}

std::pair<TextView, TextView> TextView::splitNextLine() const {
  auto pos = m_view.find_first_of('\n');
  if (pos == std::string_view::npos) {
    return {m_view, {}};
  }
  if (pos > 0 && m_view[pos - 1] == '\r') {
    return {m_view.substr(0, pos - 1), m_view.substr(pos + 1)};
  }
  return {m_view.substr(0, pos), m_view.substr(pos + 1)};
}

List<TextView> TextView::splitLines(SplitEmpty onEmpty) const {
  List<TextView> res;
  TextView view = *this;
  TextView first_line;
  while (view.size() > 0) {
    std::tie(first_line, view) = view.splitNextLine();
    if (first_line.size() > 0 || onEmpty == SplitEmpty::Keep) {
      res.add(first_line);
    }
  }
  return res;
}

List<TextView> TextView::splitByChar(char c, SplitEmpty onEmpty) const {
  List<TextView> res;
  TextView view = *this;
  TextView first_line;
  while (view.size() > 0) {
    std::tie(first_line, view) = view.splitNextChar(c);
    if (first_line.size() > 0 || onEmpty == SplitEmpty::Keep) {
      res.add(first_line);
    }
  }
  return res;
}

List<TextView> TextView::splitByText(const TextView& t, SplitEmpty onEmpty) const {
  List<TextView> res;
  TextView view = *this;
  TextView first_line;
  while (view.size() > 0) {
    auto opos = view.pos(t);
    if (opos.has_value()) {
      first_line = view.subpos(0, opos.value());
      view = view.subpos(opos.value() + t.size(), view.size());
    } else {
      first_line = view;
      view = {};
    }
    if (first_line.size() > 0 || onEmpty == SplitEmpty::Keep) {
      res.add(first_line);
    }
  }
  return res;
}

std::optional<TextView> TextView::expect(const TextView& t) const {
  if (startsWith(t)) {
    return skip(t.size());
  }
  return {};
}

std::optional<TextView> TextView::expectws(const TextView& t) const { return trim_left().expect(t); }

std::optional<TextView> TextView::skipIndent(size_t indentLevel) const {
  if (size() < indentLevel) [[unlikely]] {
    return {};
  }
  auto ptr = begin();
  for (size_t i = 0; i < indentLevel; i++) {
    if (*ptr != ' ') {
      return {};
    }
    ptr++;
  }
  return skip(indentLevel);
}

size_t TextView::getIndent() const {
  size_t level = 0;
  for (char c: *this) {
    if (c == ' ') {
      level++;
    } else {
      return level;
    }
  }
  return level;
}

size_t TextView::count(char t) const {
  size_t count = 0;
  for (auto c: *this) {
    if (c == t) {
      count++;
    }
  }
  return count;
}

TextRefCounter* TextRefCounter::acquire() {
  ref_count++;
  return this;
}
bool TextRefCounter::release() {
  ref_count--;
  return ref_count == 0;
}

char* TextRefCounter::text_data() { return reinterpret_cast<char*>(&block_start); }
TextRefCounter* TextRefCounter::allocate(size_t text_size) {
  auto buffer = reinterpret_cast<TextRefCounter*>(malloc(sizeof(TextRefCounter) + text_size));
  buffer->ref_count = 1;
  return buffer;
}

TextRefCounter TextRefCounter::s_empty;

Text::Text() : m_memblock(&TextRefCounter::s_empty), m_start(0), m_end(0) {}

Text::~Text() { reset(); }

Text::Text(const Text& t) : m_memblock(t.m_memblock->acquire()), m_start(t.m_start), m_end(t.m_end) {}

Text::Text(Text&& dying)
    : m_memblock(std::exchange(dying.m_memblock, &TextRefCounter::s_empty)), m_start(std::exchange(dying.m_start, 0)),
      m_end(std::exchange(dying.m_end, 0)) {}

Text& Text::operator=(const Text& t) {
  reset();
  if (t.size() > 0) {
    m_memblock = t.m_memblock->acquire();
    m_start = t.m_start;
    m_end = t.m_end;
  }
  return *this;
}

Text& Text::operator=(Text&& dying) {
  reset();
  if (dying.size() > 0) {
    m_memblock = std::exchange(dying.m_memblock, &TextRefCounter::s_empty);
    m_start = std::exchange(dying.m_start, 0);
    m_end = std::exchange(dying.m_end, 0);
  }
  return *this;
}

Text::Text(char c) {
  m_memblock = TextRefCounter::allocate(sizeof(c));
  m_memblock->text_data()[0] = c;
  m_end = sizeof(c);
}

Text::Text(const std::string& s) {
  m_start = 0;
  m_end = s.size();
  if (m_end > 0) {
    m_memblock = TextRefCounter::allocate(m_end);
    std::copy(s.begin(), s.end(), m_memblock->text_data());
  } else {
    m_memblock = &TextRefCounter::s_empty;
  }
}

Text::Text(const char* ptr) {
  if (ptr && (*ptr != '\0')) {
    m_end = std::strlen(ptr);
    m_memblock = TextRefCounter::allocate(m_end);
    std::copy(ptr, ptr + m_end, m_memblock->text_data());
  } else {
    m_memblock = &TextRefCounter::s_empty;
  }
}

Text::Text(const char* ptr, size_t size) {
  if (ptr && size > 0) {
    m_end = size;
    m_memblock = TextRefCounter::allocate(m_end);
    std::copy(ptr, ptr + m_end, m_memblock->text_data());
  } else {
    m_memblock = &TextRefCounter::s_empty;
  }
}

Text::Text(const Text& t, size_t start, size_t length) {
  auto real_start = std::min(t.m_start + start, t.m_end);
  auto real_end = std::min(real_start + length, t.m_end);
  if (real_start == real_end) {
    m_memblock = &TextRefCounter::s_empty;
  } else {
    m_start = real_start;
    m_end = real_end;
    m_memblock = t.m_memblock->acquire();
  }
}

Text::Text(TextRefCounter* buffer, size_t length) : m_memblock(buffer), m_end(length) {}

void Text::reset() {
  if (m_start < m_end) {
    if (m_memblock->release()) {
      free(m_memblock);
    }
    m_memblock = &TextRefCounter::s_empty;
  }
  m_start = 0;
  m_end = 0;
}

Text Text::copy() const { return Text(m_memblock->text_data() + m_start, size()); }

char Text::operator[](ssize_t index) const {
  if (index >= (ssize_t)size() || (-index > (ssize_t)size())) [[unlikely]] {
    throw std::out_of_range(fmt::format("Requested index {} out of {}", index, size()));
  }
  return *(m_memblock->text_data() + m_start + index + ((index < 0) ? size() : 0));
}

std::strong_ordering Text::operator<=>(const Text& v) const {
  const char* p = begin();
  const char* e = end();
  const char* vp = v.begin();
  const char* ve = v.end();
  while (p != e && vp != ve) {
    auto res = *p <=> *vp;
    if (res != std::strong_ordering::equal) {
      return res;
    }
    p++;
    vp++;
  }
  return size() <=> v.size();
}

std::strong_ordering Text::operator<=>(const char* v) const {
  if (v == nullptr) [[unlikely]] {
    return size() <=> 0;
  }
  const char* p = begin();
  const char* e = end();
  while (p != e) {
    auto res = *p <=> *v;
    if (res != std::strong_ordering::equal) {
      return res;
    }
    p++;
    v++;
  }
  return *v == 0 ? std::strong_ordering::equal : std::strong_ordering::less;
}

std::strong_ordering Text::operator<=>(const std::string& v) const {
  const char* p = begin();
  const char* e = end();
  auto vp = v.begin();
  auto ve = v.end();
  while (p != e && vp != ve) {
    auto res = *p <=> *vp;
    if (res != std::strong_ordering::equal) {
      return res;
    }
    p++;
    vp++;
  }
  return size() <=> v.size();
}

bool Text::operator==(const Text& tv) const {
  if (tv.size() != size()) {
    return false;
  }
  return operator<=>(tv) == std::strong_ordering::equal;
}

bool Text::operator==(const char* v) const { return operator<=>(v) == std::strong_ordering::equal; }

bool Text::operator==(const std::string& v) const {
  if (v.size() != size()) {
    return false;
  }
  return operator<=>(v) == std::strong_ordering::equal;
}

size_t Text::size() const { return m_end - m_start; }
const char* Text::begin() const { return m_memblock->text_data() + m_start; }
const char* Text::end() const { return m_memblock->text_data() + m_end; }

std::string Text::toString() const { return std::string(begin(), size()); }
std::string_view Text::toView() const { return std::string_view(begin(), size()); }
TextView Text::toTextView() const { return TextView(toView()); }
std::span<uint8_t> Text::toRawData() const { return {(uint8_t*)begin(), (uint8_t*)end()}; }
int64_t Text::toInt() const { return std::stoll(toString()); }

Text Text::trim() const { return trim_left().trim_right(); }
Text Text::trim_left() const { return skip(WHITESPACE); }
Text Text::trim_right() const {
  auto position = toView().find_last_not_of(WHITESPACE);
  if (position == std::string_view::npos) {
    return {};
  }
  return Text(*this, 0, position + 1);
}

bool Text::startsWith(const Text& tv) const { return toView().starts_with(tv.toView()); }
bool Text::startsWith(const char* v) const {
  if (v == nullptr) [[unlikely]] {
    return false;
  }
  return toView().starts_with(v);
}
bool Text::startsWith(char c) const { return size() > 0 && *begin() == c; }

bool Text::endsWith(const Text& tv) const { return toView().ends_with(tv.toView()); }
bool Text::endsWith(char c) const {
  if (size() > 0) {
    return c == *(begin() + size() - 1);
  }
  return false;
}

bool Text::contains(char c) const { return toTextView().contains(c); }

Text Text::skip(std::string_view skippables) const { return skip(toView().find_first_not_of(skippables)); }

Text Text::skip(size_t n) const {
  if (n < size()) {
    return sublen(n, m_end - n);
  };
  return {};
}

std::optional<size_t> Text::pos(char c, size_t occurence) const { return toTextView().pos(c, occurence); }

std::optional<size_t> Text::pos(Text t, size_t occurence) const { return toTextView().pos(t.toTextView(), occurence); }

std::optional<size_t> Text::lastPos(char c) const { return toTextView().lastPos(c); }

std::pair<Text, Text> Text::splitPos(ssize_t where) const {
  auto maxn = size();
  if (where < 0) {
    where += maxn;
    if (where < 0) {
      where = 0;
    }
  }
  return {sublen(0, where), sublen(where, maxn)};
}

std::pair<Text, Text> Text::splitNextChar(char c, SplitDirection direction) const {
  auto position = pos(c, 1);
  if (!position.has_value()) {
    return {*this, {}};
  }
  if (direction == SplitDirection::Discard) {
    return {sublen(0, position.value()), sublen(position.value() + 1, m_end)};
  }
  auto split_position = position.value() + ((direction == SplitDirection::KeepLeft) ? 1 : 0);
  return {sublen(0, split_position), sublen(split_position, m_end)};
}

std::pair<Text, Text> Text::splitNextLine() const {
  auto [left, right] = splitNextChar('\n', SplitDirection::Discard);
  if (left.size() > 0 && left[-1] == '\r') {
    return {left.sublen(0, left.size() - 1), right};
  }
  return {left, right};
}

List<Text> Text::splitLines(SplitEmpty onEmpty) const {
  List<Text> res;
  auto right = *this;
  Text left;
  while (right.size() > 0) {
    std::tie(left, right) = right.splitNextLine();
    if (left.size() > 0 || onEmpty == SplitEmpty::Keep) {
      res.add(left);
    }
  }
  if (onEmpty == SplitEmpty::Keep && (size() == 0 || operator[](-1) == '\n')) {
    res.add(Text{});
  }
  return res;
}

List<Text> Text::splitByChar(char c, SplitEmpty onEmpty) const {
  List<Text> res;
  auto right = *this;
  Text left;
  while (right.size() > 0) {
    std::tie(left, right) = right.splitNextChar(c, SplitDirection::Discard);
    if (left.size() > 0 || onEmpty == SplitEmpty::Keep) {
      res.add(left);
    }
  }
  if (onEmpty == SplitEmpty::Keep && (size() == 0 || operator[](-1) == c)) {
    res.add(Text{});
  }

  return res;
}

List<Text> Text::splitByText(const Text& t, SplitEmpty onEmpty) const {
  if (t.size() == 0) {
    return {*this};
  }
  List<Text> res;
  auto position = pos(t);
  auto left_over = *this;
  while (position.has_value()) {
    if (position.value() > 0 || onEmpty != SplitEmpty::Discard) {
      res.add(left_over.sublen(0, position.value()));
    }
    left_over = left_over.sublen(position.value() + t.size(), left_over.size());
    position = left_over.pos(t);
  }
  if (left_over.size() > 0 || onEmpty != SplitEmpty::Discard) {
    res.add(left_over);
  }
  return res;
}

Text Text::subpos(size_t start, size_t end) const {
  if (start >= size() || end < start) {
    return {};
  }
  end++; // include the ending character
  if (end > size()) {
    end = size();
  }
  return Text(*this, start, end - start);
}

Text Text::sublen(size_t start, size_t len) const {
  if (start >= size()) {
    return Text();
  }
  return Text(*this, start, len);
}

std::optional<Text> Text::expect(const Text& t) const {
  if (startsWith(t)) {
    return skip(t.size());
  }
  return {};
}

std::optional<Text> Text::expectws(const Text& t) const { return trim_left().expect(t); }

std::optional<Text> Text::skipIndent(size_t indentLevel) const {
  if (size() < indentLevel) [[unlikely]] {
    return {};
  }
  auto ptr = begin();
  for (size_t i = 0; i < indentLevel; i++) {
    if (*ptr != ' ') {
      return {};
    }
    ptr++;
  }
  return skip(indentLevel);
}

size_t Text::getIndent() const {
  size_t level = 0;
  for (char c: *this) {
    if (c == ' ') {
      level++;
    } else {
      return level;
    }
  }
  return level;
}

void Text::fill_c_buffer(char* dest, size_t bufsize) const {
  if (bufsize == 0) {
    return;
  }
  size_t amount_to_copy = bufsize <= size() ? bufsize - 1 : size();
  std::copy(begin(), begin() + amount_to_copy, dest);
  dest[amount_to_copy] = 0;
}

size_t Text::count(char t) const {
  return std::accumulate(begin(), end(), 0uz, [t](size_t count, char c) { return count + (c == t ? 1 : 0); });
}

size_t Text::count(Text t) const {
  return std::accumulate(begin(), end(), 0uz, [&t](size_t count, char c) { return count + (t.contains(c) ? 1 : 0); });
}

Text Text::quote_escaped() const {
  auto escapes = count("\"\\");
  if (escapes == 0) {
    return TextChain{"\"", *this, "\""};
  }
  size_t length = escapes + size() + 2;
  auto memblock = TextRefCounter::allocate(length);
  auto ptr = memblock->text_data();
  ptr[0] = '"';
  size_t offset = 1;

  for (char c: *this) {
    if (c == '"' || c == '\\') {
      ptr[offset++] = '\\';
    }
    ptr[offset++] = c;
  }
  ptr[length - 1] = '"';
  return Text(memblock, length);
}

void TextChain::_updateLength() {
  _length = 0;
  for (const auto& t: _chain) {
    _length += t.size();
  }
}

TextChain::TextChain(std::initializer_list<Text> l) : _chain(l) { _updateLength(); }
TextChain::TextChain(List<Text>&& l) : _chain(l) { _updateLength(); }
TextChain::TextChain(const List<Text>& l) : _chain(l) { _updateLength(); }
const List<Text>& TextChain::chain() const { return _chain; }
void TextChain::operator+=(const Text& text) {
  _chain.add(text);
  _length += text.size();
}

Text TextChain::toText() const {
  if (_chain.size() == 0) {
    return {};
  }
  if (_chain.size() == 1) {
    return _chain[0];
  }

  auto memblock = TextRefCounter::allocate(_length);
  char* ptr = memblock->text_data();
  uint32_t offset = 0;
  for (const auto& tv: _chain) {
    std::copy(tv.begin(), tv.end(), ptr + offset);
    offset += tv.size();
  }
  return Text(memblock, _length);
}

Text TextChain::join(char splitchar) {
  if (_length == 0) {
    return ""_t;
  }
  size_t size = _length + (splitchar != '\0' ? (_chain.size() - 1) : 0);

  auto memblock = TextRefCounter::allocate(size);
  char* ptr = memblock->text_data();

  size_t offset = 0;
  for (const auto& t: _chain) {
    if (splitchar && offset != 0) {
      ptr[offset] = splitchar;
      offset++;
    }
    std::copy(t.begin(), t.end(), ptr + offset);
    offset += t.size();
  }
  return Text(memblock, size);
}

Text TextChain::join(kl::Text split_text) {
  if (_length == 0) {
    return ""_t;
  }
  size_t size = _length + split_text.size() * (_chain.size() - 1);

  auto memblock = TextRefCounter::allocate(size);
  char* ptr = memblock->text_data();

  size_t offset = 0;
  size_t split_size = split_text.size();
  for (const auto& t: _chain) {
    if (split_size > 0 && offset != 0) {
      std::copy(split_text.begin(), split_text.end(), ptr + offset);
      offset += split_size;
    }
    std::copy(t.begin(), t.end(), ptr + offset);
    offset += t.size();
  }
  return Text(memblock, size);
}

void TextChain::add(const Text& text) {
  _chain.add(text);
  _length += text.size();
};

void TextChain::operator+=(const TextChain& text) {
  _chain.add(text._chain);
  _length += text._length;
}

void TextChain::add(const TextChain& text) {
  _chain.add(text._chain);
  _length += text._length;
};

TextChain::operator Text() const { return toText(); }
void TextChain::clear() {
  _chain = {};
  _length = 0;
}

Text Text::skipBOM() const {
  if (size() >= 3) {
    auto buf = begin();
    if (buf[0] == (char)0xEF && buf[1] == (char)0xBB && buf[2] == (char)0xBF) {
      return skip(3);
    }
  }
  return *this;
}

inline namespace literals {
Text operator"" _t(const char* p, size_t s) { return Text(p, s); }
} // namespace literals

std::ostream& operator<<(std::ostream& os, const TextView& tv) { return os << tv.view(); }
std::ostream& operator<<(std::ostream& os, const Text& tv) { return os << tv.toView(); }
std::ostream& operator<<(std::ostream& os, const TextChain& tv) { return os << tv.toText().toView(); }

} // namespace kl

std::size_t std::hash<kl::Text>::operator()(const kl::Text& s) const noexcept {
  return std::hash<std::string_view>{}(s.toView());
}

kl::TextChain operator+(const kl::Text& t, const char* p) {
  kl::TextChain tc;
  tc.add(t);
  tc.add(p);
  return tc;
}

kl::TextChain operator+(const kl::Text& t1, const kl::Text& t2) {
  kl::TextChain tc;
  tc.add(t1);
  tc.add(t2);
  return tc;
}

kl::TextChain operator+(const kl::TextChain& t1, const kl::Text& t2) {
  kl::TextChain tc;
  tc.add(t1);
  tc.add(t2);
  return tc;
}

kl::TextChain operator+(const kl::TextChain& t, const char* p) {
  kl::TextChain tc;
  tc.add(t);
  tc.add(p);
  return tc;
}
