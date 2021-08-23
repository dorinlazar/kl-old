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
  auto pv = kl::Value::createList();
  auto& v = *pv;
  CHECK(!v.isNull());
  CHECK(!v.isScalar());
  CHECK(!v.isMap());
  CHECK(v.isList());
  v.add(kl::Value::createScalar("100"_t));
  v.add(kl::Value::createScalar("200"_t));
  v.add(kl::Value::createList());
  v.add(kl::Value::createScalar("300"_t));
  v.add(kl::Value::createScalar("400"_t));
  CHECK(v.size() == 5);
  CHECK(v[0].isScalar());
  CHECK(v[0].getValue() == "100"_t);
  CHECK(v[1].isScalar());
  CHECK(v[1].getValue() == "200"_t);
  CHECK(v[2].isList());
  auto& list2 = *v.access(2);
  list2.add(kl::Value::createScalar("210"_t));
  list2.add(kl::Value::createScalar("220"_t));
  list2.add(kl::Value::createScalar("230"_t));
  list2.add(kl::Value::createScalar("240"_t));
  list2.add(kl::Value::createScalar("250"_t));
  CHECK(v[3].isScalar());
  CHECK(v[3].getValue() == "300"_t);
  CHECK(v[4].isScalar());
  CHECK(v[4].getValue() == "400"_t);
  CHECK(v[2].size() == 5);
  CHECK(v[2][0].getValue() == "210"_t);
  CHECK(v[2][1].getValue() == "220"_t);
  CHECK(v[2][2].getValue() == "230"_t);
  CHECK(v[2][3].getValue() == "240"_t);
  CHECK(v[2][4].getValue() == "250"_t);
  EXPECTEX<std::out_of_range>([&v]() { auto x = v[10]; });

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