#pragma once
#include "kltext.h"
#include "kltime.h"
#include <functional>

namespace kl {

Text getExecutablePath(const Text& exename);
bool mkDir(const Text& path);
bool isDir(const Text& path);

enum class NavigateInstructions { Continue, Skip, Stop };
enum class FileType { Directory, File };

class FilePath {
  Text _fullName;
  std::optional<uint32_t> _lastSlashPos;
  std::optional<uint32_t> _lastDotPos;
  mutable std::optional<bool> _exists;

  static Text normalize(const Text& filename);

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

  bool exists() const; // goes to the filesystem, BAD!

  std::optional<FilePath> hasFile(const FilePath& file);

  auto operator<=>(const FilePath& fp) const { return _fullName <=> fp._fullName; }
  auto operator==(const FilePath& fp) const { return _fullName == fp._fullName; }
};

struct FileInfo {
  FileType type;
  DateTime lastWrite;
  FilePath path;
};

void navigateTree(const Text& treeBase, std::function<NavigateInstructions(const FileInfo& file)>);

struct InputSource {
  virtual std::optional<Text> readLine() = 0;
  virtual std::optional<char> readChar() = 0;
  virtual List<Text> readAllLines(SplitEmpty onEmpty = SplitEmpty::Keep) = 0;
  virtual bool hasData() = 0;
};

struct FileReader : public InputSource {
  FileReader(const Text& name);
  std::optional<Text> readLine() override final;
  List<Text> readAllLines(SplitEmpty onEmpty = SplitEmpty::Keep) override final;
  std::optional<char> readChar() override final;
  bool hasData() override final;

private:
  Text _unreadContent; // we can do away with this.
};

} // namespace kl

std::ostream& operator<<(std::ostream& os, const kl::FilePath& p);
