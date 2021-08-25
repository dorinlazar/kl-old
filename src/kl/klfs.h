#pragma once
#include "kltext.h"
#include "kltime.h"
#include <functional>

namespace kl {

Text readFile(const Text& filename);
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
  FilePath(const Text& path) : _fullName(normalize(path)) {
    _lastSlashPos = _fullName.lastPos('/');
    _lastDotPos = _fullName.lastPos('.');
    if (_lastDotPos.has_value() && (*_lastDotPos == 0 || _fullName[*_lastDotPos - 1] == '/')) {
      _lastDotPos = {};
    }
  }
  Text folderName() const {
    return _lastSlashPos.has_value() ? Text(_fullName, 0, *_lastSlashPos == 0 ? 1 : *_lastSlashPos) : Text();
  }
  Text fileName() const { return _lastSlashPos.has_value() ? _fullName.skip(*_lastSlashPos + 1) : _fullName; }
  Text extension() const { return _lastDotPos.has_value() ? _fullName.skip(*_lastDotPos + 1) : Text(); }
  Text stem() const {
    uint32_t stem_start = _lastSlashPos.has_value() ? *_lastSlashPos + 1 : 0;
    uint32_t stem_end = _lastDotPos.value_or(_fullName.size());
    return Text(_fullName, stem_start, stem_end - stem_start);
  }
  Text fullPath() const { return _fullName; }

  FilePath replace_extension(const kl::Text& new_ext) const {
    if (new_ext.size() > 0) {
      if (_lastDotPos.has_value()) {
        return FilePath(_fullName.subpos(0, *_lastDotPos) + new_ext);
      }
      return FilePath(_fullName + "."_t + new_ext);
    }
    if (_lastDotPos.has_value()) {
      return FilePath(_fullName.subpos(0, *_lastDotPos - 1));
    }
    return *this;
  }

  Text baseFolder(uint32_t levels = 1) const;
  FilePath remove_base_folder(uint32_t levels = 1) const;
  FilePath replace_base_folder(const kl::Text& new_folder, uint32_t levels = 1) const;
  uint32_t depth() const { return _fullName.count('/') - (_fullName.startsWith("/") ? 1 : 0); }
  uint32_t folderDepth() const { // depth if path is folder.
    if (_fullName.size() == 0 || (_fullName.size() == 1 && _fullName[0] == '.')) {
      return 0;
    }
    return depth() + 1;
  }

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
  virtual bool hasData() = 0;
};

struct FileReader : public InputSource {
  FileReader(const Text& name);
  std::optional<Text> readLine() override final;
  std::optional<char> readChar() override final;
  bool hasData() override final;

private:
  Text _unreadContent; // we can do away with this.
};

} // namespace kl

inline std::ostream& operator<<(std::ostream& os, const kl::FilePath& p) { return os << p.fullPath(); }
