#include "kl/klvalue.h"

void test_value_null() {
  auto v = kl::Value::createNull();
  CHECK(v->isNull());
  CHECK(!v->isScalar());
  CHECK(!v->isMap());
  CHECK(!v->isList());
  kl::log("NULL test OK");
}

void test_value_scalar() {
  auto v = kl::Value::createScalar("Hello world"_t);
  CHECK(!v->isNull());
  CHECK(v->isScalar());
  CHECK(!v->isMap());
  CHECK(!v->isList());
  kl::log("SCALAR test OK");
}

void test_value_list() {
  auto v = kl::Value::createList();
  CHECK(!v->isNull());
  CHECK(!v->isScalar());
  CHECK(!v->isMap());
  CHECK(v->isList());
  kl::log("LIST test OK");
}

void test_value_map() {
  auto v = kl::Value::createMap();
  CHECK(!v->isNull());
  CHECK(!v->isScalar());
  CHECK(v->isMap());
  CHECK(!v->isList());
  kl::log("MAP test OK");
}

int main() {
  test_value_null();
  test_value_scalar();
  test_value_list();
  test_value_map();
  return 0;
}