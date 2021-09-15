#include "kltext.h"
using namespace kl;

std::shared_ptr<char> Text::s_null_data = std::make_shared<char>(0);

Text::Text(char c) {
  _memblock = std::shared_ptr<char>((char*)malloc(sizeof(c)), free);
  _memblock.get()[0] = c;
  _end = sizeof(c);
}

Text::Text(const std::string& s) {
  _end = s.size();
  if (_end > 0) {
    // TODO replace as soon as gcc implements with make_shared_for_overwrite
    _memblock = std::shared_ptr<char>((char*)malloc(_end), free);
    std::copy(s.begin(), s.end(), _memblock.get());
  }
}

Text::Text(const char* ptr) {
  if (ptr) {
    _end = std::strlen(ptr);
    if (_end > 0) {
      // TODO replace as soon as gcc implements with make_shared_for_overwrite
      _memblock = std::shared_ptr<char>((char*)malloc(_end), free);
      std::copy(ptr, ptr + _end, _memblock.get());
    }
  }
}

Text::Text(const char* ptr, uint32_t size) {
  if (ptr) {
    _end = size;
    if (_end > 0) {
      // TODO replace as soon as gcc implements with make_shared_for_overwrite
      _memblock = std::shared_ptr<char>((char*)malloc(_end), free);
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

Text Text::FromBuffer(std::shared_ptr<char> p, uint32_t start, uint32_t end) {
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
  std::shared_ptr<char> memblock = std::shared_ptr<char>((char*)malloc(_length), free);
  char* ptr = memblock.get();
  uint32_t offset = 0;
  for (const auto& tv: _chain) {
    std::copy(tv.begin(), tv.end(), ptr + offset);
    offset += tv.size();
  }
  return Text::FromBuffer(memblock, 0, _length);
}

std::ostream& std::operator<<(std::ostream& os, const kl::Text& t) { return os << t.toView(); }

TextChain operator+(const Text& t, const char* p) {
  TextChain tc;
  tc.add(t);
  tc.add(p);
  return tc;
}

TextChain operator+(const Text& t1, const Text& t2) {
  TextChain tc;
  tc.add(t1);
  tc.add(t2);
  return tc;
}

TextChain operator+(const TextChain& t1, const Text& t2) {
  TextChain tc;
  tc.add(t1);
  tc.add(t2);
  return tc;
}

TextChain operator+(const TextChain& t, const char* p) {
  TextChain tc;
  tc.add(t);
  tc.add(p);
  return tc;
}

kl::Text TextChain::join(char splitchar) {
  if (_length == 0) {
    return ""_t;
  }
  size_t size = _length + (splitchar != '\0' ? (_chain.size() - 1) : 0);

  std::shared_ptr<char> memblock = std::shared_ptr<char>((char*)malloc(size), free);

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
    auto _memblock = std::shared_ptr<char>((char*)malloc(_length), free);
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

kl::Text operator"" _t(const char* p, size_t s) { return kl::Text(p, s); }

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

std::size_t std::hash<kl::Text>::operator()(const kl::Text& s) const noexcept {
  return std::hash<std::string_view>{}(s.toView());
}
