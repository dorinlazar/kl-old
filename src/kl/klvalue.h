#pragma once

#include <variant>

#include "klds.h"
#include "kltext.h"

namespace kl {

enum class ValueType : int { Null = 0, Scalar = 1, Map = 2, List = 3 };

class Value {
public:
  using PValue = ptr<Value>;
  using ListValue = List<PValue>;
  using MapValue = Dict<Text, PValue>;
  using NullValue = struct {};

private:
  std::variant<NullValue, Text, MapValue, ListValue> _value;

public:
  Value(ValueType vt) {
    switch (vt) {
    case ValueType::Null:
      _value = NullValue();
      break;
    case ValueType::Scalar:
      _value = Text();
      break;
    case ValueType::Map:
      _value = MapValue();
      break;
    case ValueType::List:
      _value = ListValue();
      break;
    }
  }

  Value(const Text& value) { _value = value; }
  Value() { _value = NullValue(); }

  static PValue createNull() { return std::make_shared<Value>(); }
  static PValue createScalar(const Text& value = ""_t) { return std::make_shared<Value>(value); }
  static PValue createMap() { return std::make_shared<Value>(ValueType::Map); }
  static PValue createList() { return std::make_shared<Value>(ValueType::List); }

  bool isNull() const { return _value.index() == (size_t)ValueType::Null; }
  bool isScalar() const { return _value.index() == (size_t)ValueType::Scalar; }
  bool isMap() const { return _value.index() == (size_t)ValueType::Map; }
  bool isList() const { return _value.index() == (size_t)ValueType::List; }

  MapValue& asMap() {
    if (isNull()) {
      createMap();
    }
    return std::get<(int)ValueType::Map>(_value);
  }
  ListValue& asList() {
    if (isNull()) {
      createList();
    }
    return std::get<(int)ValueType::List>(_value);
  }
  const MapValue& asMap() const { return std::get<(int)ValueType::Map>(_value); }
  const ListValue& asList() const { return std::get<(int)ValueType::List>(_value); }
  Text asScalar() const { return std::get<(int)ValueType::Scalar>(_value); }

public:
  ValueType type() const { return ValueType(_value.index()); }
  TextChain toString() const;

public:
  void setValue(const Text& txt) { _value = txt; }
  Text getValue() const { return asScalar(); }

  void add(PValue v) { asList().add(v); }
  void add(const Text& txt, PValue v) { asMap().add(txt, v); }
  void add(const Text& txt) { asList().add(createScalar(txt)); }
  void add(const Text& txt, const Text& v) { asMap().add(txt, createScalar(v)); }
  void clear() {
    perform(
        nullptr, [](Text& textv) { textv = ""_t; }, [](MapValue& mapv) { mapv.clear(); },
        [](ListValue& listv) { listv.clear(); });
  }
  const Value& operator[](int index) const { return *asList()[index]; }
  const Value& operator[](const Text& key) const { return *(asMap()[key]); }
  PValue access(int index) { return asList()[index]; }
  PValue access(const Text& key) { return asMap()[key]; }
  size_t size() const {
    if (isNull()) {
      return 0;
    }
    if (isScalar()) {
      return 1;
    }
    if (isList()) {
      return std::get<(size_t)ValueType::List>(_value).size();
    }
    if (isMap()) {
      return std::get<(size_t)ValueType::Map>(_value).size();
    }
    return 0;
  }

public:
  inline void perform(std::function<void(NullValue&)> nullOp, std::function<void(Text&)> scalarOp,
                      std::function<void(MapValue&)> mapOp, std::function<void(ListValue&)> listOp) {
    if (isNull()) {
      if (nullOp) {
        nullOp(std::get<(size_t)ValueType::Null>(_value));
      }
    } else if (isScalar()) {
      if (scalarOp) {
        scalarOp(std::get<(size_t)ValueType::Scalar>(_value));
      }
    } else if (isMap()) {
      if (mapOp) {
        mapOp(std::get<(size_t)ValueType::Map>(_value));
      }
    } else if (isList()) {
      if (listOp) {
        listOp(std::get<(size_t)ValueType::List>(_value));
      }
    }
  }
};

using PValue = Value::PValue;
using ListValue = Value::ListValue;
using MapValue = Value::MapValue;
using NullValue = Value::NullValue;

} // namespace kl

inline bool operator==(const kl::Value& v, const kl::Text& t) { return v.isScalar() && v.getValue() == t; }
