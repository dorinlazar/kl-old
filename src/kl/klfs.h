#pragma once
#include "kltext.h"
#include "kltime.h"
#include <functional>

namespace kl {

class FilePath {
  Text _fullName;
  std::optional<uint32_t> _lastSlashPos;
  std::optional<uint32_t> _lastDotPos;

public:
  FilePath() = default;
  FilePath(const Text& path);
  Text folderName() const;
  Text fileName() const;
  Text extension() const;
  Text stem() const;
  Text fullPath() const;

  FilePath replace_extension(const kl::Text& new_ext) const;

  Text baseFolder(uint32_t levels = 1) const;
  FilePath remove_base_folder(uint32_t levels = 1) const;
  FilePath replace_base_folder(const kl::Text& new_folder, uint32_t levels = 1) const;

  uint32_t depth() const;
  uint32_t folderDepth() const; // depth if path is folder (usually depth()+1).

  List<Text> breadcrumbs() const;
  FilePath add(const kl::Text& component) const;

  auto operator<=>(const FilePath& fp) const { return _fullName <=> fp._fullName; }
  auto operator==(const FilePath& fp) const { return _fullName == fp._fullName; }
};

enum class FileType { Directory, File };
struct FileSystemEntryInfo {
  FileType type;
  DateTime lastWrite;
  FilePath path;
};

enum class NavigateInstructions { Continue, Skip, Stop };

struct FileSystem {
  static Text executable_path(const Text& exename);
  static bool make_directory(const Text& path);
  static bool is_directory(const Text& path);
  static bool is_file(const Text& path);
  static bool exists(const Text& path);

  static void navigate_tree(const Text& treeBase, std::function<NavigateInstructions(const FileSystemEntryInfo& file)>);
};

struct InputSource {
  virtual std::optional<Text> readLine() = 0;
  virtual std::optional<char> readChar() = 0;
  virtual List<Text> readAllLines(SplitEmpty onEmpty = SplitEmpty::Keep) = 0;
  virtual Text readAll() = 0;
  virtual bool hasData() = 0;
};

struct FileReader : public InputSource {
  FileReader(const Text& name);
  std::optional<Text> readLine() override final;
  std::optional<char> readChar() override final;
  List<Text> readAllLines(SplitEmpty onEmpty = SplitEmpty::Keep) override final;
  Text readAll() override final;
  bool hasData() override final;

private:
  Text _unreadContent; // we can do away with this.
};

struct Folder {
  const Folder* _parent = nullptr;
  kl::Text _name;
  kl::FilePath _path;
  kl::Dict<kl::Text, kl::ptr<Folder>> _folders;
  kl::List<kl::FileSystemEntryInfo> _files;

public:
  Folder() = default;
  Folder(const kl::Text& name, const kl::Text& path, const Folder* parent);
  void addItem(const kl::FileSystemEntryInfo& file, const kl::Text& path);

  kl::ptr<Folder> getFolder(const kl::Text& folder);
  kl::List<kl::ptr<Folder>> getFolders() const;
  kl::ptr<Folder> createFolder(const kl::FilePath& path);
  const kl::FilePath& fullPath() const;
  const kl::List<kl::FileSystemEntryInfo>& files() const;
  bool hasFile(const kl::Text& file) const;
};

} // namespace kl
