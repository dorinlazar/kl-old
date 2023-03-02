#pragma once
#include "kltext.h"
#include "kltime.h"
#include <functional>

namespace kl {

class FilePath {
  Text m_full_name;
  std::optional<uint32_t> m_last_slash_pos;
  std::optional<uint32_t> m_last_dot_pos;

public:
  FilePath() = default;
  FilePath(const Text& path);
  Text folderName() const;
  Text filename() const;
  Text extension() const;
  Text stem() const;
  Text fullPath() const;

  FilePath replace_extension(const kl::Text& new_ext) const;

  Text baseFolder(uint32_t levels = 1) const;
  FilePath discardBaseFolder(uint32_t levels = 1) const;
  FilePath replaceBaseFolder(const kl::Text& new_folder, uint32_t levels = 1) const;

  uint32_t depth() const;
  uint32_t folderDepth() const; // depth if path is folder (usually depth()+1).

  List<Text> breadcrumbs() const;
  FilePath add(const kl::Text& component) const;

  std::strong_ordering operator<=>(const FilePath& fp) const;
  bool operator==(const FilePath& fp) const;
};

enum class FileType { Directory, File };
struct FileSystemEntryInfo {
  FileType type;
  DateTime last_write;
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
  virtual std::optional<Text> read_line() = 0;
  virtual std::optional<char> read_char() = 0;
  virtual List<Text> read_all_lines(SplitEmpty onEmpty = SplitEmpty::Keep) = 0;
  virtual Text read_all() = 0;
  virtual bool has_data() = 0;
};

struct FileReader : public InputSource {
  FileReader(const Text& name);
  std::optional<Text> read_line() override final;
  std::optional<char> read_char() override final;
  List<Text> read_all_lines(SplitEmpty onEmpty = SplitEmpty::Keep) override final;
  Text read_all() override final;
  bool has_data() override final;

private:
  Text _unreadContent; // we can do away with this.
};

struct Folder {
  const Folder* m_parent = nullptr;
  kl::Text m_name;
  kl::FilePath m_path;
  kl::Dict<kl::Text, kl::ptr<Folder>> m_folders;
  kl::List<kl::FileSystemEntryInfo> m_files;

public:
  Folder() = default;
  Folder(const kl::Text& name, const kl::Text& path, const Folder* parent);
  void add_item(const kl::FileSystemEntryInfo& file, const kl::Text& path);

  kl::ptr<Folder> get_folder(const kl::Text& folder);
  kl::List<kl::ptr<Folder>> get_folders() const;
  kl::ptr<Folder> create_folder(const kl::FilePath& path);
  const kl::FilePath& fullPath() const;
  const kl::List<kl::FileSystemEntryInfo>& files() const;
  bool has_file(const kl::Text& file) const;
};

struct DirectoryEntry {
  kl::Text name;
  DateTime last_modified;
  size_t size;
};

class Directory {

public:
  Directory();
  Directory(Directory&&) = delete;
  Directory(const Directory&) = delete;
  Directory& operator=(Directory&&) = delete;
  Directory& operator=(const Directory&) = delete;
  ~Directory() = default;

  kl::Text name() const;
  const kl::List<kl::DirectoryEntry>& subdirectories() const;
  const kl::List<kl::DirectoryEntry>& files() const;
};

} // namespace kl
