#pragma once

#include <variant>
#include <memory>

#include "klds.h"
#include "kltext.h"

namespace kl {

enum class ValueType : int { Null = 0, Scalar = 1, Map = 2, List = 3 };

class Value {
public:
  using PValue = std::shared_ptr<Value>;
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
  void clear() { _value = NullValue(); }
  PValue operator[](int index) { return asList()[index]; }
  PValue operator[](const Text& key) { return asMap()[key]; }
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
};

using PValue = Value::PValue;
using ListValue = Value::ListValue;
using MapValue = Value::MapValue;
using NullValue = Value::NullValue;

} // namespace kl
