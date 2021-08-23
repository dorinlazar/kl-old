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
  auto pv = kl::Value::createMap();
  auto& v = *pv;
  CHECK(!v.isNull());
  CHECK(!v.isScalar());
  CHECK(v.isMap());
  CHECK(!v.isList());

  v.add("test"_t, kl::Value::createScalar("test_value"_t));
  v.add("list"_t, kl::Value::createList());
  v.add("map"_t, kl::Value::createMap());
  auto& lst = *v.access("list"_t);
  auto& m = *v.access("map"_t);
  lst.add("100"_t);
  lst.add("200"_t);
  lst.add("300"_t);
  lst.add("400"_t);
  m.add("m01", "m01_value");
  m.add("m02", "m02_value");
  m.add("m03", "m03_value");

  CHECK(v["test"].isScalar());
  CHECK(v["test"].getValue() == "test_value");

  CHECK(v["list"].isList(), "Expected list");
  CHECK(v["list"].size() == 4, "Expected list of size 4");
  CHECK(v["list"][0] == "100");
  CHECK(v["list"][1] == "200");
  CHECK(v["list"][2] == "300");
  CHECK(v["list"][3] == "400");
  CHECK(v["map"]["m01"] == "m01_value");
  CHECK(v["map"]["m02"] == "m02_value");
  CHECK(v["map"]["m03"] == "m03_value");

  EXPECTEX<std::out_of_range>([&]() { auto x = v["nonmap"]; });

  kl::log("MAP test OK");
}

int main() {
  test_value_null();
  test_value_scalar();
  test_value_list();
  test_value_map();
  return 0;
}