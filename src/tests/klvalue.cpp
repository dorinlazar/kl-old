#include "kl/klvalue.h"

void test_value_null() {
  auto v = kl::Value::createNull();
  CHECKST(v->isNull());
  CHECKST(!v->isScalar());
  CHECKST(!v->isMap());
  CHECKST(!v->isList());
  kl::log("NULL test OK");
}

void test_value_scalar() {
  auto v = kl::Value::createScalar("Hello world"_t);
  CHECKST(!v->isNull());
  CHECKST(v->isScalar());
  CHECKST(!v->isMap());
  CHECKST(!v->isList());
  kl::log("SCALAR test OK");
}

void test_value_list() {
  auto pv = kl::Value::createList();
  auto& v = *pv;
  CHECKST(!v.isNull());
  CHECKST(!v.isScalar());
  CHECKST(!v.isMap());
  CHECKST(v.isList());
  v.add(kl::Value::createScalar("100"_t));
  v.add(kl::Value::createScalar("200"_t));
  v.add(kl::Value::createList());
  v.add(kl::Value::createScalar("300"_t));
  v.add(kl::Value::createScalar("400"_t));
  CHECKST(v.size() == 5);
  CHECKST(v[0].isScalar());
  CHECKST(v[0].getValue() == "100"_t);
  CHECKST(v[1].isScalar());
  CHECKST(v[1].getValue() == "200"_t);
  CHECKST(v[2].isList());
  auto& list2 = v[2];
  list2.add(kl::Value::createScalar("210"_t));
  list2.add(kl::Value::createScalar("220"_t));
  list2.add(kl::Value::createScalar("230"_t));
  list2.add(kl::Value::createScalar("240"_t));
  list2.add(kl::Value::createScalar("250"_t));
  CHECKST(v[3].isScalar());
  CHECKST(v[3].getValue() == "300"_t);
  CHECKST(v[4].isScalar());
  CHECKST(v[4].getValue() == "400"_t);
  CHECKST(v[2].size() == 5);
  CHECKST(v[2][0].getValue() == "210"_t);
  CHECKST(v[2][1].getValue() == "220"_t);
  CHECKST(v[2][2].getValue() == "230"_t);
  CHECKST(v[2][3].getValue() == "240"_t);
  CHECKST(v[2][4].getValue() == "250"_t);
  EXPECTEX<std::out_of_range>([&v]() { auto x = v[10]; });

  kl::log("LIST test OK");
}

void test_value_map() {
  auto pv = kl::Value::createMap();
  auto& v = *pv;
  CHECKST(!v.isNull());
  CHECKST(!v.isScalar());
  CHECKST(v.isMap());
  CHECKST(!v.isList());

  v.add("test"_t, kl::Value::createScalar("test_value"_t));
  v.add("list"_t, kl::Value::createList());
  v.add("map"_t, kl::Value::createMap());
  auto& lst = v["list"_t];
  auto& m = v["map"_t];
  lst.add("100"_t);
  lst.add("200"_t);
  lst.add("300"_t);
  lst.add("400"_t);
  m.add("m01", "m01_value");
  m.add("m02", "m02_value");
  m.add("m03", "m03_value");

  CHECKST(v["test"].isScalar());
  CHECKST(v["test"].getValue() == "test_value");

  CHECKST(v["list"].isList());
  CHECKST(v["list"].size() == 4);
  CHECKST(v["list"][0] == "100");
  CHECKST(v["list"][1] == "200");
  CHECKST(v["list"][2] == "300");
  CHECKST(v["list"][3] == "400");
  CHECKST(v["map"]["m01"] == "m01_value");
  CHECKST(v["map"]["m02"] == "m02_value");
  CHECKST(v["map"]["m03"] == "m03_value");

  EXPECTEX<std::out_of_range>([&]() { auto x = v["nonmap"]; });

  kl::log("MAP test OK");
}

void test_value_getopt() {
  auto root = kl::Value::createMap();
  root->add("test"_t, kl::Value::createScalar("test_value"_t));
  root->add("map"_t, kl::Value::createMap());
  root->add("test2"_t, kl::Value::createScalar("value2"_t));
  auto m = root->get("map"_t);
  m->add("path1", kl::Value::createScalar("tv1"_t));
  m->add("path2", kl::Value::createMap());
  m->add("path3", kl::Value::createScalar("tv3"_t));
  m = m->get("path2");
  m->add("deep1", kl::Value::createScalar("dv1"_t));
  m->add("deep2", kl::Value::createScalar("dv2"_t));
  m->add("deep3", kl::Value::createScalar("dv3"_t));

  CHECKST(root->getOpt("test") == "test_value");
  CHECKST(root->getOpt("/test/") == "test_value");
  CHECKST(root->getOpt("test2") == "value2");
  CHECKST(root->getOpt("map/path1") == "tv1");
  CHECKST(root->getOpt("map/path3") == "tv3");
  CHECKST(root->getOpt("map/path3/") == "tv3");
  CHECKST(root->getOpt("map/path2/deep1") == "dv1");
  CHECKST(root->getOpt("map/path2/deep2") == "dv2");
  CHECKST(root->getOpt("map/path2/deep3") == "dv3");
  CHECKST(root->getOpt("map/path2/deep3/") == "dv3");

  CHECKST(!root->getOpt("map/path1/deep3").has_value());
  CHECKST(!root->getOpt("map2/path1/deep3").has_value());
  CHECKST(!root->getOpt("map/path2").has_value());
  CHECKST(!root->getOpt("map/").has_value());

  kl::log("VALUE GetOpt [OK]");
}

int main() {
  test_value_null();
  test_value_scalar();
  test_value_list();
  test_value_map();
  test_value_getopt();
  return 0;
}