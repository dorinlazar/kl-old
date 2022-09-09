#include "kltext.h"
#include <stdexcept>
#include <algorithm>

using namespace std::literals;

namespace kl {

constexpr std::string_view WHITESPACE = " \t\n\r"sv;

TextView::TextView(std::string_view v) : m_view(v) {}
TextView::TextView(const char* text) : m_view(text) {}
TextView::TextView(const char* text, size_t length) : m_view(text, length) {}

TextView TextView::trim() const { return trimLeft().trimRight(); }
TextView TextView::trimLeft() const { return skip(WHITESPACE); }
TextView TextView::trimRight() const {
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
  size_t pos = 0;
  while (pos < m_view.size()) {
    pos = m_view.find_first_of(c, pos);
    if (pos != std::string_view::npos) {
      occurence--;
      if (occurence == 0) {
        return pos;
      }
    }
    pos++;
  }

  return std::nullopt;
}

std::optional<size_t> TextView::pos(const TextView& t, size_t occurence) const {
  if (occurence == 0) {
    return std::nullopt;
  }
  size_t pos = 0;
  while (pos < m_view.size()) {
    pos = m_view.find(t.m_view, pos);
    if (pos < m_view.size()) {
      occurence--;
      if (occurence == 0) {
        return pos;
      }
    }
    pos += t.size();
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

std::optional<TextView> TextView::expectws(const TextView& t) const { return trimLeft().expect(t); }

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

ptr<char> Text::s_null_data = std::make_shared<char>(0);

Text::Text(char c) {
  _memblock = ptr<char>((char*)malloc(sizeof(c)), free);
  _memblock.get()[0] = c;
  _end = sizeof(c);
}

Text::Text(const std::string& s) {
  _end = s.size();
  if (_end > 0) {
    // TODO replace as soon as gcc implements with make_shared_for_overwrite
    _memblock = ptr<char>((char*)malloc(_end), free);
    std::copy(s.begin(), s.end(), _memblock.get());
  }
}

Text::Text(const char* ptr) {
  if (ptr) {
    _end = std::strlen(ptr);
    if (_end > 0) {
      // TODO replace as soon as gcc implements with make_shared_for_overwrite
      _memblock = kl::ptr<char>((char*)malloc(_end), free);
      std::copy(ptr, ptr + _end, _memblock.get());
    }
  }
}

Text::Text(const char* ptr, uint32_t size) {
  if (ptr) {
    _end = size;
    if (_end > 0) {
      // TODO replace as soon as gcc implements with make_shared_for_overwrite
      _memblock = kl::ptr<char>((char*)malloc(_end), free);
      std::copy(ptr, ptr + _end, _memblock.get());
    }
  }
}

Text::Text(const Text& t, uint32_t start, uint32_t length) {
  _memblock = t._memblock;
  _start = std::min(t._start + start, t._end);
  _end = std::min(_start + length, t._end);
  if (_start == _end) {
    _memblock = s_null_data;
    _start = 0;
    _end = 0;
  }
}

Text& Text::operator=(const Text& v) {
  _memblock = v._memblock;
  _start = v._start;
  _end = v._end;
  return *this;
}

Text Text::FromBuffer(ptr<char> p, uint32_t start, uint32_t end) {
  if (start >= end || p.get() == nullptr) {
    return {};
  }
  Text t;
  t._memblock = p;
  t._start = start;
  t._end = end;
  return t;
}

void Text::clear() {
  _memblock = s_null_data;
  _start = 0;
  _end = 0;
}

Text Text::copy() const { return Text(_memblock.get() + _start, size()); }

char Text::operator[](uint32_t index) const {
  CHECKST(index < size()); // This is brutal. Do I really need this? Alternative: return \0
  return *(_memblock.get() + _start + index);
}

uint32_t Text::size() const { return _end - _start; }
const char* Text::begin() const { return _memblock.get() ? _memblock.get() + _start : nullptr; }
const char* Text::end() const { return _memblock.get() + _end; }

std::string Text::toString() const { return std::string((const char*)_memblock.get() + _start, size()); }
std::string_view Text::toView() const { return std::string_view(_memblock.get() + _start, (size_t)size()); }

Text Text::trim() const {
  const char* ptr = (const char*)_memblock.get();
  uint32_t s = _start;
  while (s < _end && isspace(ptr[s])) {
    s++;
  }

  uint32_t e = _end;
  while (e > s && isspace(ptr[e - 1])) {
    e--;
  }

  return Text(*this, s - _start, e - s);
}

Text Text::trimLeft() const {
  const char* ptr = (const char*)_memblock.get();
  uint32_t offset = _start;
  while (offset < _end && isspace(ptr[offset])) {
    offset++;
  }
  return Text(*this, offset - _start, _end - offset);
}

Text Text::trimRight() const {
  const char* ptr = (const char*)_memblock.get();
  uint32_t offset = _end;
  while (offset > _start && isspace(ptr[offset - 1])) {
    offset--;
  }

  return Text(*this, 0, offset);
}

bool Text::startsWith(const Text& tv) const {
  if (tv.size() <= size()) [[likely]] {
    const char* p = begin();
    for (auto c: tv) {
      if (*p != c) {
        return false;
      }
      p++;
    }
    return true;
  }
  return false;
}

bool Text::startsWith(const char* v) const {
  if (v == nullptr) [[unlikely]] {
    return false;
  }
  const char* p = begin();
  const char* e = end();
  while (*v) {
    if (p == e || *p != *v) {
      return false;
    }
    v++;
    p++;
  }
  return true;
}

bool Text::endsWith(const Text& tv) const {
  if (tv.size() <= size()) [[likely]] {
    const char* p = end() - tv.size();
    for (auto c: tv) {
      if (*p != c) {
        return false;
      }
      p++;
    }
    return true;
  }
  return false;
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

bool Text::contains(char c) const {
  for (char ch: *this) {
    if (ch == c) {
      return true;
    }
  }
  return false;
}

Text Text::skip(const Text& skippables) const {
  uint32_t skipped = 0;
  const char* p = begin();
  const char* e = end();
  while (p != e) {
    if (!skippables.contains(*p)) {
      break;
    }
    skipped++;
    p++;
  }
  return Text(*this, skipped, size());
}

Text Text::skip(uint32_t n) const {
  n += _start;
  if (n >= _end) {
    return ""_t;
  }
  return Text::FromBuffer(_memblock, n, _end);
}

std::optional<uint32_t> Text::pos(char c, uint32_t occurence) const {
  if (occurence == 0) {
    return {};
  }
  uint32_t pos = 0;
  for (char x: *this) {
    if (x == c) {
      occurence--;
      if (occurence == 0) {
        return pos;
      }
    }
    pos++;
  }
  return {};
}

std::optional<uint32_t> Text::pos(Text t, uint32_t occurence) const {
  auto ts = t.size();
  auto s = size();
  if (occurence == 0 || ts == 0 || ts > s) {
    return {};
  }
  auto start = begin();
  auto tstart = t.begin();
  for (uint32_t pos = 0; pos <= s - ts; pos++) {
    if ((start[pos] == *tstart) && std::memcmp(start + pos, tstart, ts) == 0) {
      occurence--;
      if (occurence == 0) {
        return pos;
      }
    }
  }
  return {};
}

std::optional<uint32_t> Text::lastPos(char c) const {
  uint32_t pos = size();
  while (pos > 0) {
    pos--;
    if (_memblock.get()[pos + _start] == c) {
      return pos;
    }
  }
  return {};
}

std::pair<Text, Text> Text::splitPos(int32_t where) const {
  int32_t maxn = size();
  if (where < 0) {
    where += maxn;
    if (where < 0) {
      where = 0;
    }
  }
  if (where > maxn) {
    where = maxn;
  }
  return {Text(*this, 0, where), Text(*this, where, maxn)};
}

std::pair<Text, Text> Text::splitNextChar(char c, SplitDirection direction) const {
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset < _end; current_offset++) {
    if (ptr[current_offset] == c) {
      if (direction == SplitDirection::KeepLeft) {
        current_offset++;
      }
      auto first = Text::FromBuffer(_memblock, _start, current_offset);
      if (direction == SplitDirection::Discard) {
        current_offset++;
      }
      return {first, Text::FromBuffer(_memblock, current_offset, _end)};
    }
  }
  return {*this, Text()};
}

std::pair<Text, Text> Text::splitNextLine() const {
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset < _end; current_offset++) {
    if (ptr[current_offset] == '\n') {
      auto first = Text::FromBuffer(_memblock, _start, current_offset);
      if (current_offset < _end - 1 && (ptr[current_offset + 1] == '\r')) {
        current_offset++;
      }
      current_offset++;
      return {first, Text::FromBuffer(_memblock, current_offset, _end)};
    } else if (ptr[current_offset] == '\r') {
      auto first = Text::FromBuffer(_memblock, _start, current_offset);
      if (current_offset < _end - 1 && (ptr[current_offset + 1] == '\n')) {
        current_offset++;
      }
      current_offset++;
      return {first, Text::FromBuffer(_memblock, current_offset, _end)};
    }
  }
  return {*this, Text()};
}

List<Text> Text::splitLines(SplitEmpty onEmpty) const {
  List<Text> res;
  uint32_t last_start = _start;
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset < _end; current_offset++) {
    if (ptr[current_offset] == '\n') { // Time to build the next return item
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.add(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      if ((current_offset < _end - 1) && (ptr[current_offset + 1] == '\r')) {
        current_offset++;
      }
      last_start = current_offset + 1;
    } else if (ptr[current_offset] == '\r') {
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.add(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      if ((current_offset < _end - 1) && (ptr[current_offset + 1] == '\n')) {
        current_offset++;
      }
      last_start = current_offset + 1;
    }
  }
  if (_end > last_start || onEmpty == SplitEmpty::Keep) {
    res.add(Text::FromBuffer(_memblock, last_start, _end));
  }
  return res;
}

List<Text> Text::splitByChar(char c, SplitEmpty onEmpty) const {
  List<Text> res;
  uint32_t last_start = _start;
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset < _end; current_offset++) {
    if (ptr[current_offset] == c) { // Time to build the next return item
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.add(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      last_start = current_offset + 1;
    }
  }
  if (_end > last_start || onEmpty == SplitEmpty::Keep) {
    res.add(Text::FromBuffer(_memblock, last_start, _end));
  }
  return res;
}

List<Text> Text::splitByText(const Text& t, SplitEmpty onEmpty) const {
  if (t.size() == 0) {
    return {*this};
  }
  if (t.size() >= size()) {
    return (t == *this) ? (onEmpty == SplitEmpty::Keep ? List<Text>({""_t}) : List<Text>()) : List<Text>({*this});
  }
  List<Text> res;
  uint32_t last_start = _start;
  char c = t[0];
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset <= _end - t.size(); current_offset++) {
    if (ptr[current_offset] == c &&
        std::memcmp(t.begin(), ptr + current_offset, t.size()) == 0) { // Time to build the next return item
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.add(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      last_start = current_offset + t.size();
      current_offset += t.size() - 1;
    }
  }
  if (_end > last_start || onEmpty == SplitEmpty::Keep) {
    res.add(Text::FromBuffer(_memblock, last_start, _end));
  }
  return res;
}

Text TextChain::toText() const {
  if (_chain.size() == 0) {
    return {};
  }
  if (_chain.size() == 1) {
    return _chain[0];
  }
  // TODO replace as soon as gcc implements with make_shared_for_overwrite
  ptr<char> memblock = ptr<char>((char*)malloc(_length), free);
  char* ptr = memblock.get();
  uint32_t offset = 0;
  for (const auto& tv: _chain) {
    std::copy(tv.begin(), tv.end(), ptr + offset);
    offset += tv.size();
  }
  return Text::FromBuffer(memblock, 0, _length);
}

kl::Text TextChain::join(char splitchar) {
  if (_length == 0) {
    return ""_t;
  }
  size_t size = _length + (splitchar != '\0' ? (_chain.size() - 1) : 0);

  ptr<char> memblock = ptr<char>((char*)malloc(size), free);

  size_t offset = 0;
  char* p = memblock.get();
  for (const auto& t: _chain) {
    if (splitchar && offset != 0) {
      p[offset] = splitchar;
      offset++;
    }
    std::copy(t.begin(), t.end(), p + offset);
    offset += t.size();
  }
  return kl::Text::FromBuffer(memblock, 0, (uint32_t)size);
}

void TextChain::consolidate() {
  // TODO replace as soon as gcc implements with make_shared_for_overwrite
  if (_length > 0) {
    List<Text> new_chain(_chain.size());
    auto _memblock = kl::ptr<char>((char*)malloc(_length), free);
    char* ptr = _memblock.get();
    uint32_t offset = 0;
    for (uint32_t i = 0; i < _chain.size(); i++) {
      auto& t = _chain[i];
      if (t.size() > 0) {
        std::copy(t.begin(), t.end(), ptr + offset);
        new_chain.add(std::move(Text::FromBuffer(_memblock, offset, offset + t.size())));
        offset += t.size();
      }
    }
    _chain = new_chain;
  }
}

int64_t Text::toInt() const {
  // TODO make this properly
  return std::stoll(toString());
}

Text Text::subpos(uint32_t start, uint32_t end) const {
  if (start >= size() || end < start) {
    return Text();
  }
  end++; // include the ending character
  if (end > size()) {
    end = size();
  }
  return Text::FromBuffer(_memblock, _start + start, _start + end);
}

Text Text::sublen(uint32_t start, uint32_t len) const {
  if (start >= size()) {
    return Text();
  }
  if (start + len > size()) {
    len = size() - start;
  }
  return Text::FromBuffer(_memblock, _start + start, _start + start + len);
}

std::optional<Text> Text::expect(const Text& t) const {
  if (startsWith(t)) {
    return skip(t.size());
  }
  return {};
}

std::optional<Text> Text::expectws(const Text& t) const { return trimLeft().expect(t); }

std::optional<Text> Text::skipIndent(uint32_t indentLevel) const {
  if (size() < indentLevel) [[unlikely]] {
    return {};
  }
  auto ptr = begin();
  for (uint32_t i = 0; i < indentLevel; i++) {
    if (*ptr != ' ') {
      return {};
    }
    ptr++;
  }
  return skip(indentLevel);
}

uint32_t Text::getIndent() const {
  uint32_t level = 0;
  for (char c: *this) {
    if (c == ' ') {
      level++;
    } else {
      return level;
    }
  }
  return level;
}

void Text::fill_c_buffer(char* dest, uint32_t bufsize) const {
  if (bufsize == 0) {
    return;
  }
  uint32_t amount_to_copy = bufsize <= size() ? bufsize - 1 : size();
  std::copy(begin(), begin() + amount_to_copy, dest);
  dest[amount_to_copy] = 0;
}

uint32_t Text::count(char t) const {
  uint32_t count = 0;
  for (auto c: *this) {
    if (c == t) {
      count++;
    }
  }
  return count;
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

std::span<uint8_t> Text::toRawData() const { return {(uint8_t*)begin(), (uint8_t*)end()}; }
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
kl::Text operator"" _t(const char* p, size_t s) { return kl::Text(p, s); }
} // namespace literals

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
