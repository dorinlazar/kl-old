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
  Text folder_name() const;
  Text filename() const;
  Text extension() const;
  Text stem() const;
  Text full_path() const;

  FilePath replace_extension(const kl::Text& new_ext) const;

  Text base_folder(uint32_t levels = 1) const;
  FilePath remove_base_folder(uint32_t levels = 1) const;
  FilePath replace_base_folder(const kl::Text& new_folder, uint32_t levels = 1) const;

  uint32_t depth() const;
  uint32_t folder_depth() const; // depth if path is folder (usually depth()+1).

  List<Text> breadcrumbs() const;
  FilePath add(const kl::Text& component) const;

  std::strong_ordering operator<=>(const FilePath& fp) const;
  bool operator==(const FilePath& fp) const;
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
  kl::FilePath _path;
  kl::Dict<kl::Text, kl::ptr<Folder>> m_folders;
  kl::List<kl::FileSystemEntryInfo> m_files;

public:
  Folder() = default;
  Folder(const kl::Text& name, const kl::Text& path, const Folder* parent);
  void add_item(const kl::FileSystemEntryInfo& file, const kl::Text& path);

  kl::ptr<Folder> get_folder(const kl::Text& folder);
  kl::List<kl::ptr<Folder>> get_folders() const;
  kl::ptr<Folder> create_folder(const kl::FilePath& path);
  const kl::FilePath& full_path() const;
  const kl::List<kl::FileSystemEntryInfo>& files() const;
  bool has_file(const kl::Text& file) const;
};

} // namespace kl
