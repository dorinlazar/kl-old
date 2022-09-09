#include "kl/klvalue.h"
#include <gtest/gtest.h>
using namespace kl::literals;

TEST(klvalue, test_value_null) {
  auto v = kl::Value::createNull();
  EXPECT_TRUE(v->isNull());
  EXPECT_TRUE(!v->isScalar());
  EXPECT_TRUE(!v->isMap());
  EXPECT_TRUE(!v->isList());
}

TEST(klvalue, test_value_scalar) {
  auto v = kl::Value::createScalar("Hello world"_t);
  EXPECT_TRUE(!v->isNull());
  EXPECT_TRUE(v->isScalar());
  EXPECT_TRUE(!v->isMap());
  EXPECT_TRUE(!v->isList());
}

TEST(klvalue, test_value_list) {
  auto pv = kl::Value::createList();
  auto& v = *pv;
  EXPECT_TRUE(!v.isNull());
  EXPECT_TRUE(!v.isScalar());
  EXPECT_TRUE(!v.isMap());
  EXPECT_TRUE(v.isList());
  v.add(kl::Value::createScalar("100"_t));
  v.add(kl::Value::createScalar("200"_t));
  v.add(kl::Value::createList());
  v.add(kl::Value::createScalar("300"_t));
  v.add(kl::Value::createScalar("400"_t));
  EXPECT_TRUE(v.size() == 5);
  EXPECT_TRUE(v[0].isScalar());
  EXPECT_TRUE(v[0].getValue() == "100"_t);
  EXPECT_TRUE(v[1].isScalar());
  EXPECT_TRUE(v[1].getValue() == "200"_t);
  EXPECT_TRUE(v[2].isList());
  auto& list2 = v[2];
  list2.add(kl::Value::createScalar("210"_t));
  list2.add(kl::Value::createScalar("220"_t));
  list2.add(kl::Value::createScalar("230"_t));
  list2.add(kl::Value::createScalar("240"_t));
  list2.add(kl::Value::createScalar("250"_t));
  EXPECT_TRUE(v[3].isScalar());
  EXPECT_TRUE(v[3].getValue() == "300"_t);
  EXPECT_TRUE(v[4].isScalar());
  EXPECT_TRUE(v[4].getValue() == "400"_t);
  EXPECT_TRUE(v[2].size() == 5);
  EXPECT_TRUE(v[2][0].getValue() == "210"_t);
  EXPECT_TRUE(v[2][1].getValue() == "220"_t);
  EXPECT_TRUE(v[2][2].getValue() == "230"_t);
  EXPECT_TRUE(v[2][3].getValue() == "240"_t);
  EXPECT_TRUE(v[2][4].getValue() == "250"_t);
  EXPECT_THROW(v[10], std::out_of_range);
}

TEST(klvalue, test_value_map) {
  auto pv = kl::Value::createMap();
  auto& v = *pv;
  EXPECT_TRUE(!v.isNull());
  EXPECT_TRUE(!v.isScalar());
  EXPECT_TRUE(v.isMap());
  EXPECT_TRUE(!v.isList());

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

  EXPECT_TRUE(v["test"].isScalar());
  EXPECT_TRUE(v["test"].getValue() == "test_value");

  EXPECT_TRUE(v["list"].isList());
  EXPECT_TRUE(v["list"].size() == 4);
  EXPECT_TRUE(v["list"][0] == "100");
  EXPECT_TRUE(v["list"][1] == "200");
  EXPECT_TRUE(v["list"][2] == "300");
  EXPECT_TRUE(v["list"][3] == "400");
  EXPECT_TRUE(v["map"]["m01"] == "m01_value");
  EXPECT_TRUE(v["map"]["m02"] == "m02_value");
  EXPECT_TRUE(v["map"]["m03"] == "m03_value");

  EXPECT_THROW(v["nonmap"], std::out_of_range);
}

TEST(klvalue, test_value_getopt) {
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

  EXPECT_TRUE(root->getOpt("test") == "test_value");
  EXPECT_TRUE(root->getOpt("/test/") == "test_value");
  EXPECT_TRUE(root->getOpt("test2") == "value2");
  EXPECT_TRUE(root->getOpt("map/path1") == "tv1");
  EXPECT_TRUE(root->getOpt("map/path3") == "tv3");
  EXPECT_TRUE(root->getOpt("map/path3/") == "tv3");
  EXPECT_TRUE(root->getOpt("map/path2/deep1") == "dv1");
  EXPECT_TRUE(root->getOpt("map/path2/deep2") == "dv2");
  EXPECT_TRUE(root->getOpt("map/path2/deep3") == "dv3");
  EXPECT_TRUE(root->getOpt("map/path2/deep3/") == "dv3");

  EXPECT_TRUE(!root->getOpt("map/path1/deep3").has_value());
  EXPECT_TRUE(!root->getOpt("map2/path1/deep3").has_value());
  EXPECT_TRUE(!root->getOpt("map/path2").has_value());
  EXPECT_TRUE(!root->getOpt("map/").has_value());
}
