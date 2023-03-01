#include "kl/klfs.h"
#include <gtest/gtest.h>
using namespace kl::literals;

TEST(klfs, test_file_path) {
  kl::FilePath path("/foldername/sub/folder/file.name.txt");
  EXPECT_EQ(path.stem(), "file.name");
  EXPECT_EQ(path.filename(), "file.name.txt");
  EXPECT_EQ(path.extension(), "txt");
  EXPECT_EQ(path.folderName(), "/foldername/sub/folder");
  EXPECT_EQ(path.baseFolder(), "/foldername");
  EXPECT_EQ(path.baseFolder(2), "/foldername/sub");
  EXPECT_EQ(path.baseFolder(3), "/foldername/sub/folder");
  EXPECT_EQ(path.baseFolder(10), "/foldername/sub/folder");

  path = kl::FilePath("/foldername/sub/folder/.filenametxt");
  EXPECT_EQ(path.stem(), ".filenametxt");
  EXPECT_EQ(path.filename(), ".filenametxt");
  EXPECT_EQ(path.extension(), ""_t);
  EXPECT_EQ(path.folderName(), "/foldername/sub/folder");
}

TEST(klfs, test_replace_components) {
  kl::FilePath p("/test1/file.txt"_t);
  EXPECT_EQ(p.replace_extension("cpp").fullPath(), "/test1/file.cpp"_t);
  EXPECT_EQ(p.replace_extension("cpp").extension(), "cpp"_t);
  EXPECT_EQ(p.replace_extension("").fullPath(), "/test1/file"_t);
  kl::FilePath p2("/test1/.file"_t);
  EXPECT_EQ(p2.replace_extension("cpp"_t).fullPath(), "/test1/.file.cpp"_t);
  EXPECT_EQ(p2.replace_extension(""_t).fullPath(), "/test1/.file"_t);
  kl::FilePath p3("file"_t);
  kl::FilePath p4("/1/2/3/4/5/6/7/file"_t);
  kl::FilePath p5("1/2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.baseFolder(), "1");
  EXPECT_EQ(p5.baseFolder(2), "1/2");
  EXPECT_EQ(p5.baseFolder(3), "1/2/3");
  EXPECT_EQ(p.depth(), 1);
  EXPECT_EQ(p.discardBaseFolder(3).fullPath(), "file.txt"_t);
  EXPECT_EQ(p.discardBaseFolder(0).fullPath(), "/test1/file.txt"_t);
  EXPECT_EQ(p.discardBaseFolder(1).fullPath(), "file.txt"_t);
  EXPECT_EQ(p.discardBaseFolder(2).fullPath(), "file.txt"_t);
  EXPECT_EQ(p3.depth(), 0);
  EXPECT_EQ(p3.discardBaseFolder(3), p3);
  EXPECT_EQ(p3.discardBaseFolder(0), p3);
  EXPECT_EQ(p4.depth(), 7);
  EXPECT_EQ(p4.discardBaseFolder(0).fullPath(), "/1/2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(1).fullPath(), "2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(2).fullPath(), "3/4/5/6/7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(3).fullPath(), "4/5/6/7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(4).fullPath(), "5/6/7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(5).fullPath(), "6/7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(6).fullPath(), "7/file"_t);
  EXPECT_EQ(p4.discardBaseFolder(7).fullPath(), "file"_t);
  EXPECT_EQ(p4.discardBaseFolder(8).fullPath(), "file"_t);
  EXPECT_EQ(p5.depth(), 7);
  EXPECT_EQ(p5.discardBaseFolder(0).fullPath(), "1/2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(1).fullPath(), "2/3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(2).fullPath(), "3/4/5/6/7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(3).fullPath(), "4/5/6/7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(4).fullPath(), "5/6/7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(5).fullPath(), "6/7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(6).fullPath(), "7/file"_t);
  EXPECT_EQ(p5.discardBaseFolder(7).fullPath(), "file"_t);
  EXPECT_EQ(p5.discardBaseFolder(8).fullPath(), "file"_t);

  EXPECT_EQ(p.replaceBaseFolder("x"), "x/file.txt"_t);
  EXPECT_EQ(p.replaceBaseFolder("x", 0), "x/test1/file.txt"_t);
  EXPECT_EQ(p3.replaceBaseFolder("x", 0), "x/file"_t);
  EXPECT_EQ(p3.replaceBaseFolder("x"), "x/file"_t);
  EXPECT_EQ(p5.replaceBaseFolder("x/y/z", 3), "x/y/z/4/5/6/7/file"_t);
  EXPECT_EQ(p4.replaceBaseFolder("x/y/z", 3), "x/y/z/4/5/6/7/file"_t);
}

TEST(klfs, test_file_path_normalization) {
  kl::FilePath path("/");
  EXPECT_EQ(path.fullPath(), "/"_t);
  path = kl::FilePath("/hello/world/!!!");
  EXPECT_EQ(path.fullPath(), "/hello/world/!!!"_t);
  path = kl::FilePath("/hello/world/!!!/");
  EXPECT_EQ(path.fullPath(), "/hello/world/!!!"_t);
  path = kl::FilePath("hello/world/!!!/");
  EXPECT_EQ(path.fullPath(), "hello/world/!!!"_t);
  path = kl::FilePath("./hello/world/!!!/");
  EXPECT_EQ(path.fullPath(), "hello/world/!!!"_t);
  path = kl::FilePath("./hello///world//!!!/");
  EXPECT_EQ(path.fullPath(), "hello/world/!!!"_t);
  path = kl::FilePath("////////");
  EXPECT_EQ(path.fullPath(), "/"_t);
  path = kl::FilePath("./");
  EXPECT_EQ(path.fullPath(), "."_t);
  path = kl::FilePath("./////");
  EXPECT_EQ(path.fullPath(), "."_t);
  path = kl::FilePath("./////");
  EXPECT_EQ(path.fullPath(), "."_t);
}
