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
url: https://example.com\n\
name: example.com\n\
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
  CHECKST(map["url"]->getValue() == "https://example.com");

  CHECKST(map["name"]->isScalar());
  CHECKST(map["name"]->getValue() == "example.com");

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

void test_comments() {
  auto value = kl::PoorConfig::parse("\n\
url: https://example.com # the URL: hello world\n\
name: example.com#how tight can we make this?\n\
        #naturally a comment $$\n\
params:\n\
  test: test-value\n\
#a badly indented comment? \n\
  test2: test2-value\n\
  test3:\n\
    test: subvalue\n\n\n\
    test2: subvalue2\n\
   # another badly indented comment\n\
  test4: test4-value\n\
test-up: back on track!!!");
  auto map = value->asMap();
  CHECKST(map.size() == 4);
  CHECKST(map.has("url"));
  CHECKST(map.has("name"));
  CHECKST(map.has("params"));
  CHECKST(map.has("test-up"));

  CHECKST(map["url"]->isScalar());
  CHECKST(map["url"]->getValue() == "https://example.com");

  CHECKST(map["name"]->isScalar());
  CHECKST(map["name"]->getValue() == "example.com");

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

  kl::log("POORCONFIG Comments [OK]");
}

void test_list() {
  auto value = kl::PoorConfig::parse(R"(url: "https://example.com"
names: ["name01", "name02"]
deeper:
  x: y
  y: ["hello", "world"
, "mean", "world"
] #discardable junk
  z: we keep having this conversation
a-list: [
  "hello",
  "dear",
  "listeners"
]
empty: []
last: ["value"])");
  CHECKST(value->isMap());
  CHECKST(value->size() == 6);
  auto map = value->asMap();
  CHECKST(map.size() == 6);
  CHECKST(map.has("url"));
  CHECKST(map.has("names"));
  CHECKST(map.has("deeper"));
  CHECKST(map.has("a-list"));
  CHECKST(map.has("empty"));
  CHECKST(map.has("last"));

  CHECKST((*value)["url"].getValue() == "https://example.com");

  CHECKST(map["names"]->isList());
  const auto& list1 = map["names"]->asList();
  CHECKST(list1.size() == 2);
  CHECKST(list1[0]->getValue() == "name01");
  CHECKST(list1[1]->getValue() == "name02");

  CHECKST(map["deeper"]->isMap());
  auto& deeper = map["deeper"]->asMap();
  CHECKST(deeper.size() == 3);
  CHECKST(deeper["x"]->getValue() == "y");
  CHECKST(deeper["y"]->isList());
  const auto& y = deeper["y"]->asList();
  CHECKST(y.size() == 4);
  CHECKST(y[0]->getValue() == "hello");
  CHECKST(y[1]->getValue() == "world");
  CHECKST(y[2]->getValue() == "mean");
  CHECKST(y[3]->getValue() == "world");
  CHECKST(deeper["z"]->getValue() == "we keep having this conversation");

  CHECKST(map["a-list"]->isList());
  const auto& alist = map["a-list"]->asList();
  CHECKST(alist.size() == 3);
  CHECKST(alist[0]->getValue() == "hello");
  CHECKST(alist[1]->getValue() == "dear");
  CHECKST(alist[2]->getValue() == "listeners");

  CHECKST(map["empty"]->isList());
  CHECKST(map["empty"]->asList().size() == 0);

  CHECKST(map["last"]->isList());
  CHECKST(map["last"]->asList().size() == 1);
  CHECKST(map["last"]->asList()[0]->getValue() == "value");

  kl::log("POORCONFIG Basic List tests [OK]");
}

void test_partial_content() {
  auto value = kl::PoorConfig::parse(R"(---
url: "https://example.com"
names: ["name01", "name02"]
---
deeper:
  x: y
  y: ["hello", "world"
, "mean", "world"
] #discardable junk
  z: we keep having this conversation
a-list: [
  "hello",
  "dear",
  "listeners"
]
empty: []
last: ["value"])");
  CHECKST(value->isMap());
  CHECKST(value->size() == 2);
  auto map = value->asMap();
  CHECKST(map.size() == 2);
  CHECKST(map.has("url"));
  CHECKST(map.has("names"));

  CHECKST((*value)["url"].getValue() == "https://example.com");

  CHECKST(map["names"]->isList());
  const auto& list1 = map["names"]->asList();
  CHECKST(list1.size() == 2);
  CHECKST(list1[0]->getValue() == "name01");
  CHECKST(list1[1]->getValue() == "name02");

  kl::TextScanner scanner(R"(---
url: "https://example.com"
names: ["name01", "name02"]
---
deeper:)");
  value = kl::PoorConfig::parse(scanner);
  CHECKST((*value)["url"].getValue() == "https://example.com");
  CHECKST(scanner.startsWith("deeper:"_t));

  kl::TextScanner scanner1(R"(---
url: "https://example.com"
names: ["name01", "name02"]
...
deeper:)");
  value = kl::PoorConfig::parse(scanner1);
  CHECKST((*value)["url"].getValue() == "https://example.com");
  CHECKST(scanner1.startsWith("deeper:"_t));

  kl::TextScanner scanner2(R"(---
url: "https://example.com"
names: ["name01", "name02"]
...deeper:)");
  value = kl::PoorConfig::parse(scanner2);
  CHECKST((*value)["url"].getValue() == "https://example.com");
  CHECKST(scanner2.startsWith("deeper:"_t));

  kl::TextScanner scanner3(R"(---
url: "https://example.com"
names: ["name01", "name02"]
---deeper:)");
  value = kl::PoorConfig::parse(scanner3);
  CHECKST((*value)["url"].getValue() == "https://example.com");
  CHECKST(scanner3.startsWith("deeper:"_t));

  kl::log("POORCONFIG Multicontent partial test [OK]");
}

void test_invalid_config_01() {
  try {
    auto value = kl::PoorConfig::parse("\nurl\nname: example.com\n");
  } catch (const kl::ParsingError& fmtError) {
    CHECKST(fmtError.line() == 2);
    CHECKST(fmtError.column() == 4);
  }
  kl::log("POORCONFIG Invalid Config [OK]");
}

int main() {
  test_basic();
  test_newlines();
  test_empty();
  test_multi_level_values();
  test_invalid_config_01();
  test_comments();
  test_list();
  test_partial_content();
}
