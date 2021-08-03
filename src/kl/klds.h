#pragma once

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <exception>
#include <stdexcept>
#include <functional>
#include <optional>

namespace kl {

template <typename T>
class List {
  std::vector<T> _vec;

public:
  List() {}
  List(std::initializer_list<T> v) : _vec(v) {}
  List(size_t size) { _vec.reserve(size); }
  T& operator[](size_t pos) {
    if (pos < 0 || pos >= _vec.size()) {
      throw std::out_of_range("Invalid key");
    }
    return _vec[pos];
  }
  const T& operator[](size_t pos) const {
    if (pos < 0 || pos >= _vec.size()) {
      throw std::out_of_range("Invalid key");
    }
    return _vec[pos];
  }

  auto begin() { return _vec.begin(); }
  auto end() { return _vec.end(); }
  auto begin() const { return _vec.begin(); }
  auto end() const { return _vec.end(); }

  size_t size() const { return _vec.size(); }

  void add(T&& value) { _vec.emplace_back(std::move(value)); }
  void add(const T& value) { _vec.push_back(value); }
  void add(const List<T>& other) {
    _vec.reserve(size() + other.size());
    for (const auto& t: other) {
      _vec.push_back(t);
    }
  }
  void add(std::initializer_list<T> other) {
    _vec.reserve(size() + other.size());
    for (const auto& t: other) {
      _vec.push_back(t);
    }
  }

  void forEach(std::function<void(const T&)> op) const {
    for (const auto& item: _vec) {
      op(item);
    }
  }

  template <typename U>
  List<U> transform(std::function<U(const T&)> tr) const {
    List<U> res(size());
    for (const auto& item: _vec) {
      res.add(tr(item));
    }
    return res;
  }

  List<T>& sortInPlace() {
    std::sort(_vec.begin(), _vec.end());
    return *this;
  }

  // TODO UTs
  void remove(const T& value) { _vec.erase(std::remove(_vec.begin(), _vec.end(), value), _vec.end()); }
  void removeAt(size_t index) {
    if (index >= 0 && index < size()) {
      auto it = _vec.begin() + index;
      _vec.erase(it, it + 1);
    }
  }
  void removeRange(size_t index, size_t rangeSize) {
    if (index >= 0 && index < size() && rangeSize > 0) {
      auto it = _vec.begin() + index;
      auto offset = index + rangeSize;
      auto endit = offset >= _vec.size() ? _vec.end() : _vec.begin() + offset;
      _vec.erase(it, endit);
    }
  }
};

template <typename T1, typename T2>
class Pair {
  std::pair<T1, T2> _pair;

public:
  Pair(T1&& t1, T2&& t2) { _pair = std::make_pair(std::move(t1), std::move(t2)); }
  Pair(T1 t1, T2 t2) { _pair = std::make_pair(t1, t2); }
  Pair(Pair&&) = default;
  Pair(const Pair&) = default;
  ~Pair() = default;

  auto first() { return _pair.first(); }
  auto first() const { return _pair.first(); }
  auto second() { return _pair.second(); }
  auto second() const { return _pair.second(); }
};

template <typename T>
class Queue {
  std::deque<T> _queue;

public:
  Queue() = default;
  void push(const T& elem) { _queue.push_back(elem); }
  void push(T&& elem) { _queue.emplace_back(std::move(elem)); }
  void push(const List<T>& elem) {
    for (const auto& item: elem) {
      _queue.push_back(item);
    }
  }
  T pop() {
    CHECKST(_queue.size() != 0);
    T value = _queue.front();
    _queue.pop_front();
    return value;
  }
  bool empty() const { return _queue.size() == 0; }
  bool has(const T& value) {
    for (const auto& v: _queue) {
      if (v == value) {
        return true;
      }
    }
    return false;
  }
};

template <typename K, typename V>
class Dict {
  std::unordered_map<K, V> _map;

public:
  Dict() {}
  Dict(const List<Pair<K, V>>& pairList) {
    _map.reserve(pairList.size());
    for (const auto& it: pairList) {
      add(it.first(), it.second());
    }
  }

  auto begin() const { return _map.cbegin(); }
  auto end() const { return _map.cend(); }
  auto begin() { return _map.begin(); }
  auto end() { return _map.end(); }

  size_t size() const { return _map.size(); }

  V& operator[](const K& key) {
    auto it = _map.find(key);
    if (it == _map.end()) {
      throw std::out_of_range("Invalid key");
    }
    return it->second;
  }

  const V& get(const K& key, const V& default_value = V()) const {
    auto it = _map.find(key);
    if (it == _map.end()) {
      return default_value;
    }
    return it->second;
  }

  const std::optional<V> getOpt(const K& key) const {
    auto it = _map.find(key);
    if (it == _map.end()) {
      return {};
    }
    return it->second;
  }

  void add(const K& key, V&& value) { _map.insert_or_assign(key, std::move(value)); }
  void add(const K& key, const V& value) { _map.insert_or_assign(key, value); }

  void remove(const K& key) { _map.erase(key); }

  bool has(const K& key) const { return _map.contains(key); }

  List<K> keys() const {
    List<K> list(_map.size());
    for (const auto& [k, v]: _map) {
      list.add(k);
    }
    return list;
  }
  List<V> values() const {
    List<V> list(_map.size());
    for (const auto& [k, v]: _map) {
      list.add(v);
    }
    return list;
  }
};

template <typename T1, typename T2>
class List<Pair<T1, T2>> {
public:
  Dict<T1, T2> toDict() { return Dict(*this); }
};

template <typename T>
class Set {
  std::unordered_set<T> _data;

public:
  Set() {}
  Set(std::initializer_list<T> v) : _data(v) {}

  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }
  auto begin() { return _data.begin(); }
  auto end() { return _data.end(); }

  size_t size() const { return _data.size(); }

  void add(T&& value) { _data.insert(std::move(value)); }
  void add(const T& value) { _data.insert(value); }
  void add(const List<T>& other) {
    for (const auto& t: other) {
      _data.insert(t);
    }
  }
  void add(const Set<T>& other) {
    for (const auto& t: other) {
      _data.insert(t);
    }
  }
  bool has(const T& v) { return _data.contains(v); }

  void forEach(std::function<void(const T&)> op) const {
    for (const auto& item: _data) {
      op(item);
    }
  }

  List<T> toList() const {
    List<T> lst(size());
    for (const auto& item: _data) {
      lst.add(item);
    }
    return lst;
  }
};

} // namespace kl

template <typename K, typename V>
inline std::ostream& std::operator<<(std::ostream& os, const kl::Dict<K, V>& d) {
  os << "{";
  bool commaNeeded = false;
  for (const auto& [k, v]: d) {
    if (commaNeeded) {
      os << ",";
    } else {
      commaNeeded = true;
    }
    os << k << ":" << v;
  }
  return os << "}";
}

template <typename T>
inline std::ostream& std::operator<<(std::ostream& os, const kl::List<T>& l) {
  os << "[";
  bool commaNeeded = false;
  for (const auto& t: l) {
    if (commaNeeded) {
      os << ",";
    } else {
      commaNeeded = true;
    }
    os << t;
  }
  return os << "]";
}

template <typename T>
inline std::ostream& std::operator<<(std::ostream& os, const kl::Set<T>& l) {
  os << "[";
  bool commaNeeded = false;
  for (const auto& t: l) {
    if (commaNeeded) {
      os << ",";
    } else {
      commaNeeded = true;
    }
    os << t;
  }
  return os << "]";
}
