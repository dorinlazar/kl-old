#include "ff/poorconfig.h"

void test_basic() {
  auto value = kl::PoorConfig::parse("CXXFLAGS = -std=c++20 -flto\nLDFLAGS=-flto\nJOBS=6", '=');
  CHECKST(value->isMap());
  auto map = value->asMap();
  CHECKST(map.size() == 3);
  CHECKST(map.has("CXXFLAGS"));
  CHECKST(map["CXXFLAGS"]->isScalar());
  CHECKST(map["CXXFLAGS"]->getValue() == "-std=c++20 -flto");
  CHECKST(map.has("LDFLAGS"));
  CHECKST(map["LDFLAGS"]->isScalar());
  CHECKST(map["LDFLAGS"]->getValue() == "-flto");
  CHECKST(map.has("JOBS"));
  CHECKST(map["JOBS"]->isScalar());
  CHECKST(map["JOBS"]->getValue() == "6");
  kl::log("POORCONFIG Basic test [OK]");
}

void test_newlines() {
  auto value = kl::PoorConfig::parse("\n\n\nCXXFLAGS = -std=c++20 -flto\n\nLDFLAGS=-flto\n\nJOBS=6\n\n\n", '=');
  CHECKST(value->isMap());
  auto map = value->asMap();
  CHECKST(map.size() == 3);
  CHECKST(map["CXXFLAGS"]->getValue() == "-std=c++20 -flto");
  CHECKST(map["LDFLAGS"]->getValue() == "-flto");
  CHECKST(map["JOBS"]->getValue() == "6");
  kl::log("POORCONFIG New lines [OK]");
}

void test_empty() {
  auto value = kl::PoorConfig::parse("", '=');
  CHECKST(value->isMap());
  auto map = value->asMap();
  CHECKST(map.size() == 0);
  kl::log("POORCONFIG Empty [OK]");
}

void test_multi_level_values() {
  auto value = kl::PoorConfig::parse("\n\
url: https://dorinlazar.ro\n\
name: dorinlazar.ro\n\
params:\n\
  test: test-value\n\
  test2: test2-value\n\
  test3:\n\
    test: subvalue\n\
    test2: subvalue2\n\
  test4: test4-value\n\
test-up: back on track!!!");
  auto map = value->asMap();
  CHECKST(map.size() == 4);
  CHECKST(map.has("url"));
  CHECKST(map.has("name"));
  CHECKST(map.has("params"));
  CHECKST(map.has("test-up"));

  CHECKST(map["url"]->isScalar());
  CHECKST(map["url"]->getValue() == "https://dorinlazar.ro");

  CHECKST(map["name"]->isScalar());
  CHECKST(map["name"]->getValue() == "dorinlazar.ro");

  CHECKST(map["params"]->isMap());
  auto& params = map["params"]->asMap();
  CHECKST(params["test"]->getValue() == "test-value");
  CHECKST(params["test2"]->getValue() == "test2-value");
  CHECKST(params["test3"]->isMap());
  auto& test3 = params["test3"]->asMap();
  CHECKST(test3["test"]->getValue() == "subvalue");
  CHECKST(test3["test2"]->getValue() == "subvalue2");

  CHECKST(params["test4"]->getValue() == "test4-value");

  CHECKST(map["test-up"]->isScalar());
  CHECKST(map["test-up"]->getValue() == "back on track!!!");

  kl::log("POORCONFIG Multilevel values [OK]");
}

void test_invalid_config_01() {
  try {
    auto value = kl::PoorConfig::parse("\nurl\nname: dorinlazar.ro\n");
  } catch (const kl::ParsingError& fmtError) {
    CHECKST(fmtError.line() == 2);
  }
}

int main() {
  test_basic();
  test_newlines();
  test_empty();
  test_multi_level_values();
  test_invalid_config_01();
}
