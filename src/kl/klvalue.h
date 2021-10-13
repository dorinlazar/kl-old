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
  Value(ValueType vt);
  Value(const Text& value);
  Value();

  static PValue createNull();
  static PValue createScalar(const Text& value = ""_t);
  static PValue createMap();
  static PValue createList();

  bool isNull() const;
  bool isScalar() const;
  bool isMap() const;
  bool isList() const;

  MapValue& asMap();
  ListValue& asList();
  const MapValue& asMap() const;
  const ListValue& asList() const;
  Text asScalar() const;

public:
  ValueType type() const;
  TextChain toString() const;

public:
  void setValue(const Text& txt);
  Text getValue() const;
  List<Text> getArrayValue() const;

  void add(PValue v);
  void add(const Text& txt, PValue v);
  void add(const Text& txt);
  void add(const Text& txt, const Text& v);
  void clear();
  Value& operator[](int index) const;
  Value& operator[](const Text& key) const;
  PValue get(int index) const;
  PValue get(const Text& key) const;
  bool has(const Text& key) const;
  size_t size() const;
  std::optional<Text> getOpt(const kl::Text& path);

public:
  inline void perform(std::function<void(NullValue&)> nullOp, std::function<void(Text&)> scalarOp,
                      std::function<void(MapValue&)> mapOp, std::function<void(ListValue&)> listOp);
};

using PValue = Value::PValue;
using ListValue = Value::ListValue;
using MapValue = Value::MapValue;
using NullValue = Value::NullValue;

} // namespace kl

bool operator==(const kl::Value& v, const kl::Text& t);
