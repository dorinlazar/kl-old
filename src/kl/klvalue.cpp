#include "klvalue.h"

using namespace kl;

TextChain Value::toString() const {
  TextChain tc;
  bool commaNeeded = false;
  switch (type()) {
  case ValueType::Null:
    return {"null"_t};
  case ValueType::Scalar:
    return {"\""_t, asScalar(), "\""_t};
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
