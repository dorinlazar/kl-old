#pragma once

#include <variant>
#include <memory>

#include "klds.h"
#include "kltext.h"

namespace kl {

enum class ValueType : int { Null = 0, Scalar = 1, Map = 2, Array = 3 };

class Value {
public:
  using PValue = std::shared_ptr<Value>;
  using ArrayValue = List<PValue>;
  using MapValue = Dict<Text, PValue>;
  using NullValue = struct {};

private:
  std::variant<NullValue, Text, MapValue, ArrayValue> _value;

  MapValue&         _as_map() { return std::get<(int)ValueType::Map>(_value); }
  const MapValue&   _as_cmap() const { return std::get<(int)ValueType::Map>(_value); }
  ArrayValue&       _as_array() { return std::get<(int)ValueType::Array>(_value); }
  const ArrayValue& _as_carray() const { return std::get<(int)ValueType::Array>(_value); }
  Text              _as_scalar() const { return std::get<(int)ValueType::Scalar>(_value); }
  bool              isNull() { return _value.index() == (int)ValueType::Null; }

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
    case ValueType::Array:
      _value = ArrayValue();
      break;
    }
  }

  Value(const Text& value) { _value = value; }
  Value() { _value = NullValue(); }

  static PValue createNull() { return std::make_shared<Value>(); }
  static PValue createScalar(const Text& value = ""_t) { return std::make_shared<Value>(value); }
  static PValue createMap() { return std::make_shared<Value>(ValueType::Map); }
  static PValue createArray() { return std::make_shared<Value>(ValueType::Array); }

public:
  ValueType type() const { return ValueType(_value.index()); }
  TextChain toString() const;
};

using PValue = Value::PValue;
using ArrayValue = Value::ArrayValue;
using MapValue = Value::MapValue;
using NullValue = Value::NullValue;

} // namespace kl
