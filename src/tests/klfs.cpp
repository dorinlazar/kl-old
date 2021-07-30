#include "kl/klfs.h"

void test_file_path() {
  kl::FilePath path("/foldername/sub/folder/file.name.txt");
  CHECKST(path.stem() == "file.name");
  CHECKST(path.fileName() == "file.name.txt");
  CHECKST(path.extension() == "txt");
  CHECKST(path.folderName() == "/foldername/sub/folder");
  CHECKST(path.baseFolder() == "/foldername");
  CHECKST(path.baseFolder(2) == "/foldername/sub");
  CHECKST(path.baseFolder(3) == "/foldername/sub/folder");
  CHECKST(path.baseFolder(10) == "/foldername/sub/folder");

  path = kl::FilePath("/foldername/sub/folder/.filenametxt");
  CHECKST(path.stem() == ".filenametxt");
  CHECKST(path.fileName() == ".filenametxt");
  CHECKST(path.extension() == ""_t);
  CHECKST(path.folderName() == "/foldername/sub/folder");

  kl::log("FilePath [OK]");
}

void test_replace_components() {
  kl::FilePath p("/test1/file.txt"_t);
  CHECKST(p.replace_extension("cpp").fullPath() == "/test1/file.cpp"_t);
  CHECKST(p.replace_extension("cpp").extension() == "cpp"_t);
  CHECKST(p.replace_extension("").fullPath() == "/test1/file"_t);
  kl::FilePath p2("/test1/.file"_t);
  CHECKST(p2.replace_extension("cpp"_t).fullPath() == "/test1/.file.cpp"_t);
  CHECKST(p2.replace_extension(""_t).fullPath() == "/test1/.file"_t);
  kl::FilePath p3("file"_t);
  kl::FilePath p4("/1/2/3/4/5/6/7/file"_t);
  kl::FilePath p5("1/2/3/4/5/6/7/file"_t);
  CHECKST(p5.baseFolder() == "1");
  CHECKST(p5.baseFolder(2) == "1/2");
  CHECKST(p5.baseFolder(3) == "1/2/3");
  CHECKST(p.depth() == 1);
  CHECKST(p.remove_base_folder(3).fullPath() == "file.txt"_t);
  CHECKST(p.remove_base_folder(0).fullPath() == "/test1/file.txt"_t);
  CHECKST(p.remove_base_folder(1).fullPath() == "file.txt"_t);
  CHECKST(p.remove_base_folder(2).fullPath() == "file.txt"_t);
  CHECKST(p3.depth() == 0);
  CHECKST(p3.remove_base_folder(3) == p3);
  CHECKST(p3.remove_base_folder(0) == p3);
  CHECKST(p4.depth() == 7);
  CHECKST(p4.remove_base_folder(0).fullPath() == "/1/2/3/4/5/6/7/file"_t);
  CHECKST(p4.remove_base_folder(1).fullPath() == "2/3/4/5/6/7/file"_t);
  CHECKST(p4.remove_base_folder(2).fullPath() == "3/4/5/6/7/file"_t);
  CHECKST(p4.remove_base_folder(3).fullPath() == "4/5/6/7/file"_t);
  CHECKST(p4.remove_base_folder(4).fullPath() == "5/6/7/file"_t);
  CHECKST(p4.remove_base_folder(5).fullPath() == "6/7/file"_t);
  CHECKST(p4.remove_base_folder(6).fullPath() == "7/file"_t);
  CHECKST(p4.remove_base_folder(7).fullPath() == "file"_t);
  CHECKST(p4.remove_base_folder(8).fullPath() == "file"_t);
  CHECKST(p5.depth() == 7);
  CHECKST(p5.remove_base_folder(0).fullPath() == "1/2/3/4/5/6/7/file"_t);
  CHECKST(p5.remove_base_folder(1).fullPath() == "2/3/4/5/6/7/file"_t);
  CHECKST(p5.remove_base_folder(2).fullPath() == "3/4/5/6/7/file"_t);
  CHECKST(p5.remove_base_folder(3).fullPath() == "4/5/6/7/file"_t);
  CHECKST(p5.remove_base_folder(4).fullPath() == "5/6/7/file"_t);
  CHECKST(p5.remove_base_folder(5).fullPath() == "6/7/file"_t);
  CHECKST(p5.remove_base_folder(6).fullPath() == "7/file"_t);
  CHECKST(p5.remove_base_folder(7).fullPath() == "file"_t);
  CHECKST(p5.remove_base_folder(8).fullPath() == "file"_t);

  CHECKST(p.replace_base_folder("x") == "x/file.txt"_t);
  CHECKST(p.replace_base_folder("x", 0) == "x/test1/file.txt"_t);
  CHECKST(p3.replace_base_folder("x", 0) == "x/file"_t);
  CHECKST(p3.replace_base_folder("x") == "x/file"_t);
  CHECKST(p5.replace_base_folder("x/y/z", 3) == "x/y/z/4/5/6/7/file"_t);
  CHECKST(p4.replace_base_folder("x/y/z", 3) == "x/y/z/4/5/6/7/file"_t);

  kl::log("FilePath replace components [OK]");
}

void test_file_path_normalization() {
  kl::FilePath path("/");
  CHECKST(path.fullPath() == "/"_t);
  path = kl::FilePath("/hello/world/!!!");
  CHECKST(path.fullPath() == "/hello/world/!!!"_t);
  path = kl::FilePath("/hello/world/!!!/");
  CHECKST(path.fullPath() == "/hello/world/!!!"_t);
  path = kl::FilePath("hello/world/!!!/");
  CHECKST(path.fullPath() == "hello/world/!!!"_t);
  path = kl::FilePath("./hello/world/!!!/");
  CHECKST(path.fullPath() == "hello/world/!!!"_t);
  path = kl::FilePath("./hello///world//!!!/");
  CHECKST(path.fullPath() == "hello/world/!!!"_t);
  path = kl::FilePath("////////");
  CHECKST(path.fullPath() == "/"_t);
  path = kl::FilePath("./");
  CHECKST(path.fullPath() == "."_t);
  path = kl::FilePath("./////");
  CHECKST(path.fullPath() == "."_t);
  path = kl::FilePath("./////");
  CHECKST(path.fullPath() == "."_t);
  kl::log("FilePath normalization [OK]");
}

int main() {
  test_file_path();
  test_file_path_normalization();
  test_replace_components();
  return 0;
}
