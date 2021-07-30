#include "klvalue.h"

using namespace kl;

TextChain Value::toString() const {
  TextChain tc;
  bool      commaNeeded = false;
  switch (type()) {
  case ValueType::Null:
    return {"null"_t};
  case ValueType::Scalar:
    return {"\""_t, _as_scalar(), "\""_t};
  case ValueType::Array:
    tc.add("["_t);
    for (const auto& v: _as_carray()) {
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
    for (const auto& [k, v]: _as_cmap()) {
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
