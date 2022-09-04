#include "kl/klfs.h"
#include <gtest/gtest.h>
using namespace kl::literals;

TEST(klfs, test_file_path) {
  kl::FilePath path("/foldername/sub/folder/file.name.txt");
  EXPECT_TRUE(path.stem() == "file.name");
  EXPECT_TRUE(path.fileName() == "file.name.txt");
  EXPECT_TRUE(path.extension() == "txt");
  EXPECT_TRUE(path.folderName() == "/foldername/sub/folder");
  EXPECT_TRUE(path.baseFolder() == "/foldername");
  EXPECT_TRUE(path.baseFolder(2) == "/foldername/sub");
  EXPECT_TRUE(path.baseFolder(3) == "/foldername/sub/folder");
  EXPECT_TRUE(path.baseFolder(10) == "/foldername/sub/folder");

  path = kl::FilePath("/foldername/sub/folder/.filenametxt");
  EXPECT_TRUE(path.stem() == ".filenametxt");
  EXPECT_TRUE(path.fileName() == ".filenametxt");
  EXPECT_TRUE(path.extension() == ""_t);
  EXPECT_TRUE(path.folderName() == "/foldername/sub/folder");
}

TEST(klfs, test_replace_components) {
  kl::FilePath p("/test1/file.txt"_t);
  EXPECT_TRUE(p.replace_extension("cpp").fullPath() == "/test1/file.cpp"_t);
  EXPECT_TRUE(p.replace_extension("cpp").extension() == "cpp"_t);
  EXPECT_TRUE(p.replace_extension("").fullPath() == "/test1/file"_t);
  kl::FilePath p2("/test1/.file"_t);
  EXPECT_TRUE(p2.replace_extension("cpp"_t).fullPath() == "/test1/.file.cpp"_t);
  EXPECT_TRUE(p2.replace_extension(""_t).fullPath() == "/test1/.file"_t);
  kl::FilePath p3("file"_t);
  kl::FilePath p4("/1/2/3/4/5/6/7/file"_t);
  kl::FilePath p5("1/2/3/4/5/6/7/file"_t);
  EXPECT_TRUE(p5.baseFolder() == "1");
  EXPECT_TRUE(p5.baseFolder(2) == "1/2");
  EXPECT_TRUE(p5.baseFolder(3) == "1/2/3");
  EXPECT_TRUE(p.depth() == 1);
  EXPECT_TRUE(p.remove_base_folder(3).fullPath() == "file.txt"_t);
  EXPECT_TRUE(p.remove_base_folder(0).fullPath() == "/test1/file.txt"_t);
  EXPECT_TRUE(p.remove_base_folder(1).fullPath() == "file.txt"_t);
  EXPECT_TRUE(p.remove_base_folder(2).fullPath() == "file.txt"_t);
  EXPECT_TRUE(p3.depth() == 0);
  EXPECT_TRUE(p3.remove_base_folder(3) == p3);
  EXPECT_TRUE(p3.remove_base_folder(0) == p3);
  EXPECT_TRUE(p4.depth() == 7);
  EXPECT_TRUE(p4.remove_base_folder(0).fullPath() == "/1/2/3/4/5/6/7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(1).fullPath() == "2/3/4/5/6/7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(2).fullPath() == "3/4/5/6/7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(3).fullPath() == "4/5/6/7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(4).fullPath() == "5/6/7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(5).fullPath() == "6/7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(6).fullPath() == "7/file"_t);
  EXPECT_TRUE(p4.remove_base_folder(7).fullPath() == "file"_t);
  EXPECT_TRUE(p4.remove_base_folder(8).fullPath() == "file"_t);
  EXPECT_TRUE(p5.depth() == 7);
  EXPECT_TRUE(p5.remove_base_folder(0).fullPath() == "1/2/3/4/5/6/7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(1).fullPath() == "2/3/4/5/6/7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(2).fullPath() == "3/4/5/6/7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(3).fullPath() == "4/5/6/7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(4).fullPath() == "5/6/7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(5).fullPath() == "6/7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(6).fullPath() == "7/file"_t);
  EXPECT_TRUE(p5.remove_base_folder(7).fullPath() == "file"_t);
  EXPECT_TRUE(p5.remove_base_folder(8).fullPath() == "file"_t);

  EXPECT_TRUE(p.replace_base_folder("x") == "x/file.txt"_t);
  EXPECT_TRUE(p.replace_base_folder("x", 0) == "x/test1/file.txt"_t);
  EXPECT_TRUE(p3.replace_base_folder("x", 0) == "x/file"_t);
  EXPECT_TRUE(p3.replace_base_folder("x") == "x/file"_t);
  EXPECT_TRUE(p5.replace_base_folder("x/y/z", 3) == "x/y/z/4/5/6/7/file"_t);
  EXPECT_TRUE(p4.replace_base_folder("x/y/z", 3) == "x/y/z/4/5/6/7/file"_t);
}

TEST(klfs, test_file_path_normalization) {
  kl::FilePath path("/");
  EXPECT_TRUE(path.fullPath() == "/"_t);
  path = kl::FilePath("/hello/world/!!!");
  EXPECT_TRUE(path.fullPath() == "/hello/world/!!!"_t);
  path = kl::FilePath("/hello/world/!!!/");
  EXPECT_TRUE(path.fullPath() == "/hello/world/!!!"_t);
  path = kl::FilePath("hello/world/!!!/");
  EXPECT_TRUE(path.fullPath() == "hello/world/!!!"_t);
  path = kl::FilePath("./hello/world/!!!/");
  EXPECT_TRUE(path.fullPath() == "hello/world/!!!"_t);
  path = kl::FilePath("./hello///world//!!!/");
  EXPECT_TRUE(path.fullPath() == "hello/world/!!!"_t);
  path = kl::FilePath("////////");
  EXPECT_TRUE(path.fullPath() == "/"_t);
  path = kl::FilePath("./");
  EXPECT_TRUE(path.fullPath() == "."_t);
  path = kl::FilePath("./////");
  EXPECT_TRUE(path.fullPath() == "."_t);
  path = kl::FilePath("./////");
  EXPECT_TRUE(path.fullPath() == "."_t);
}
