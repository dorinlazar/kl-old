#include "klvalue.h"
#include "klfs.h"
using namespace kl;

Value::Value(ValueType vt) {
  switch (vt) {
  case ValueType::Null: _value = NullValue(); break;
  case ValueType::Scalar: _value = Text(); break;
  case ValueType::Map: _value = MapValue(); break;
  case ValueType::List: _value = ListValue(); break;
  }
}
Value::Value(const Text& value) { _value = value; }
Value::Value() { _value = NullValue(); }

PValue Value::createNull() { return std::make_shared<Value>(); }
PValue Value::createScalar(const Text& value) { return std::make_shared<Value>(value); }
PValue Value::createMap() { return std::make_shared<Value>(ValueType::Map); }
PValue Value::createList() { return std::make_shared<Value>(ValueType::List); }

bool Value::isNull() const { return _value.index() == (size_t)ValueType::Null; }
bool Value::isScalar() const { return _value.index() == (size_t)ValueType::Scalar; }
bool Value::isMap() const { return _value.index() == (size_t)ValueType::Map; }
bool Value::isList() const { return _value.index() == (size_t)ValueType::List; }

MapValue& Value::asMap() {
  if (isNull()) {
    createMap();
  }
  return std::get<(int)ValueType::Map>(_value);
}
ListValue& Value::asList() {
  if (isNull()) {
    createList();
  }
  return std::get<(int)ValueType::List>(_value);
}
const MapValue& Value::asMap() const { return std::get<(int)ValueType::Map>(_value); }
const ListValue& Value::asList() const { return std::get<(int)ValueType::List>(_value); }
Text Value::asScalar() const { return std::get<(int)ValueType::Scalar>(_value); }

ValueType Value::type() const { return ValueType(_value.index()); }
void Value::setValue(const Text& txt) { _value = txt; }
Text Value::getValue() const { return asScalar(); }
List<Text> Value::getArrayValue() const {
  if (isScalar()) {
    return {asScalar()};
  }
  return asList().transform<Text>([](const PValue& p) { return p->asScalar(); });
}

void Value::add(PValue v) { asList().add(v); }
void Value::add(const Text& txt, PValue v) { asMap().add(txt, v); }
void Value::add(const Text& txt) { asList().add(createScalar(txt)); }
void Value::add(const Text& txt, const Text& v) { asMap().add(txt, createScalar(v)); }
void Value::clear() {
  perform(
      nullptr, [](Text& textv) { textv = ""_t; }, [](MapValue& mapv) { mapv.clear(); },
      [](ListValue& listv) { listv.clear(); });
}
Value& Value::operator[](int index) const { return *asList()[index]; }
Value& Value::operator[](const Text& key) const { return *(asMap()[key]); }
PValue Value::get(int index) const { return asList()[index]; }
PValue Value::get(const Text& key) const { return asMap()[key]; }
bool Value::has(const Text& key) const { return asMap().has(key); }

size_t Value::size() const {
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

inline void Value::perform(std::function<void(NullValue&)> nullOp, std::function<void(Text&)> scalarOp,
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

TextChain Value::toString() const {
  TextChain tc;
  bool commaNeeded = false;
  switch (type()) {
  case ValueType::Null: return {"null"_t};
  case ValueType::Scalar: return {"\""_t, asScalar(), "\""_t};
  case ValueType::List:
    tc.add("["_t);
    for (const auto& v: asList()) {
      if (commaNeeded) {
        tc.add(","_t);
      } else {
        commaNeeded = true;
      }
      tc.add(v->toString());
    }
    tc.add("]"_t);
    break;
  case ValueType::Map:
    tc.add("{"_t);
    for (const auto& [k, v]: asMap()) {
      if (commaNeeded) {
        tc.add(","_t);
      } else {
        commaNeeded = true;
      }
      tc.add(k);
      tc.add(":"_t);
      tc.add(v->toString());
    }
    tc.add("}"_t);
    break;
  }
  return tc;
}

std::optional<Text> Value::getOpt(const kl::Text& path) {
  Value* v = this;
  for (const auto& val: kl::FilePath(path).breadcrumbs()) {
    if (v->isMap()) {
      auto& map = v->asMap();
      v = map.get(val).get();
    } else {
      v = nullptr;
    }
    if (v == nullptr) {
      break;
    }
  }
  if (v != nullptr && v->isScalar()) {
    return v->getValue();
  }
  return {};
}

bool operator==(const kl::Value& v, const kl::Text& t) { return v.isScalar() && v.getValue() == t; }
