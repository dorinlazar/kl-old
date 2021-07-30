#include "kltext.h"
using namespace kl;

std::shared_ptr<char> Text::s_null_data = std::make_shared<char>(0);

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

std::tuple<Text, Text> Text::splitPos(int32_t where) const {
  uint32_t maxn = size();
  if (where < 0) {
    where += maxn;
    if (where < 0) {
      where = 0;
    }
  }
  if (where > maxn) {
    where = maxn;
  }
  return std::make_tuple(Text(*this, 0, where), Text(*this, where, maxn));
}

std::vector<Text> Text::splitLines(SplitEmpty onEmpty) const {
  std::vector<Text> res;
  uint32_t last_start = _start;
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset < _end; current_offset++) {
    if (ptr[current_offset] == '\n') { // Time to build the next return item
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.push_back(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      if ((current_offset < _end - 1) && (ptr[current_offset + 1] == '\r')) {
        current_offset++;
      }
      last_start = current_offset + 1;
    } else if (ptr[current_offset] == '\r') {
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.push_back(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      if ((current_offset < _end - 1) && (ptr[current_offset + 1] == '\n')) {
        current_offset++;
      }
      last_start = current_offset + 1;
    }
  }
  if (_end > last_start || onEmpty == SplitEmpty::Keep) {
    res.push_back(Text::FromBuffer(_memblock, last_start, _end));
  }
  return res;
}

std::vector<Text> Text::splitByChar(char c, SplitEmpty onEmpty) const {
  std::vector<Text> res;
  uint32_t last_start = _start;
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset < _end; current_offset++) {
    if (ptr[current_offset] == c) { // Time to build the next return item
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.push_back(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      last_start = current_offset + 1;
    }
  }
  if (_end > last_start || onEmpty == SplitEmpty::Keep) {
    res.push_back(Text::FromBuffer(_memblock, last_start, _end));
  }
  return res;
}

std::vector<Text> Text::splitByText(const Text& t, SplitEmpty onEmpty) const {
  std::vector<Text> res;
  if (t.size() >= size()) {
    return (t == *this) ? (onEmpty == SplitEmpty::Keep ? std::vector<Text>({""_t}) : std::vector<Text>())
                        : std::vector<Text>({*this});
  }
  if (t.size() == 0) {
    return {*this};
  }
  uint32_t last_start = _start;
  char c = t[0];
  const char* ptr = _memblock.get();
  for (uint32_t current_offset = _start; current_offset <= _end - t.size(); current_offset++) {
    if (ptr[current_offset] == c &&
        std::memcmp(t.begin(), ptr + current_offset, t.size()) == 0) { // Time to build the next return item
      if (current_offset > last_start || onEmpty == SplitEmpty::Keep) {
        res.push_back(Text::FromBuffer(_memblock, last_start, current_offset));
      }
      last_start = current_offset + t.size();
      current_offset += t.size() - 1;
    }
  }
  if (_end > last_start || onEmpty == SplitEmpty::Keep) {
    res.push_back(Text::FromBuffer(_memblock, last_start, _end));
  }
  return res;
}

Text TextChain::toText() const {
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
