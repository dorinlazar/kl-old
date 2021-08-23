#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>

#include "klfs.h"

using namespace kl;
namespace fs = std::filesystem;

Text discardable_folders[] = {"."_t, ".."_t};
Text folder_separator("/");

Text kl::readFile(const Text& filename) {
  fs::path p = filename.startsWith("/") ? filename.toView() : fs::current_path() / filename.toView();
  std::error_code ec;
  auto size = fs::file_size(p, ec);
  if (ec) {
    err("Encountered error while checking", filename, ":", ec.message());
    return ""_t;
  }
  std::ifstream is(p, std::ifstream::binary);
  char a, b, c;
  a = is.get();
  b = is.get();
  c = is.get();
  if (a != (char)0xEF || b != (char)0xBB || c != (char)0xBF) {
    is.seekg(0);
  } else {
    size -= 3;
  }

  if (size > 0x8FFFFFFFULL) {
    // we don't work with oversized data
    log("File size for", filename.toView(), "exceeds expectations:", size);
    return ""_t;
  }
  std::shared_ptr<char> memblock = std::shared_ptr<char>((char*)malloc(size), free);
  is.read(memblock.get(), size);
  return Text::FromBuffer(memblock, 0, (uint32_t)size);
}

Text FilePath::normalize(const Text& filename) {
  TextChain tc;
  uint32_t lastPos = 0;
  bool lastWasSlash = false;
  uint32_t lastSlashPos = 0;
  bool cutNeeded = false;
  for (uint32_t i = 0; i < filename.size(); i++) {
    if (filename[i] == '/') {
      if (lastWasSlash) {
        cutNeeded = true;
      } else {
        lastWasSlash = true;
        cutNeeded = false;
        lastSlashPos = i;
      }
    } else {
      if (cutNeeded) {
        tc.add(filename.subpos(lastPos, lastSlashPos));
        lastPos = i;
      }
      cutNeeded = false;
      lastWasSlash = false;
    }
  }

  if (lastWasSlash) {
    tc.add(filename.subpos(lastPos, lastSlashPos == 0 ? 0 : lastSlashPos - 1)); // don't include the ending /
    lastPos = filename.size();
  }
  tc.add(filename.skip(lastPos));
  Text res = tc.toText();
  if (res.startsWith("./") && res.size() > 2) {
    res = res.skip(2);
  }
  return res;
}

Text FilePath::baseFolder(uint32_t levels) const {
  if (levels == 0 || _fullName.size() == 0) {
    return ""_t;
  }
  if (levels < depth()) {
    if (_fullName[0] == '/') {
      levels++;
    }
    auto pos = _fullName.pos('/', levels);
    if (pos.has_value()) {
      return _fullName.subpos(0, *pos - 1);
    }
  }
  return folderName();
}

FilePath FilePath::remove_base_folder(uint32_t levels) const {
  if (levels == 0 || _fullName.size() == 0) {
    return *this;
  }
  if (levels < depth()) {
    Text p = _fullName[0] == '/' ? _fullName.skip(1) : _fullName;
    auto pos = p.pos('/', levels);
    if (pos.has_value()) {
      return p.subpos(*pos + 1, p.size());
    }
  }
  return FilePath(fileName());
}

FilePath FilePath::replace_base_folder(const kl::Text& new_folder, uint32_t levels) const {
  FilePath fp = remove_base_folder(levels);
  return FilePath(new_folder + folder_separator + fp._fullName);
}

bool FilePath::exists() const {
  if (!_exists.has_value()) {
    _exists = fs::exists(_fullName.toView());
  }
  return *_exists;
}

std::optional<FilePath> FilePath::hasFile(const FilePath& file) {
  if (file._fullName.startsWith(_fullName)) {
    return file.remove_base_folder(folderDepth());
  }
  return {};
}

std::vector<FileInfo> _get_directory_entries(const Text& folder) {
  std::vector<FileInfo> res;
  char buffer[1024];
  folder.fill_c_buffer(buffer, 1024);
  DIR* dir = opendir(buffer);
  dirent* de;
  Text padded_folder = folder + folder_separator;
  while ((de = readdir(dir))) {
    Text t(de->d_name);
    if (t == discardable_folders[0] || t == discardable_folders[1]) {
      continue;
    }
    if (de->d_type == DT_REG || de->d_type == DT_DIR || de->d_type == DT_LNK) {
      Text full_path = padded_folder + t;
      full_path.fill_c_buffer(buffer, 1024);
      struct stat statbuf;
      FileType ft = FileType::Directory;
      if (0 == stat(buffer, &statbuf)) {
        if (S_ISREG(statbuf.st_mode)) {
          ft = FileType::File;
        } else if (S_ISDIR(statbuf.st_mode)) {
          ft = FileType::Directory;
        } else {
          continue;
        }
        DateTime lastWrite(statbuf.st_mtim.tv_sec, statbuf.st_mtim.tv_nsec);
        FileInfo fi{.type = ft, .lastWrite = lastWrite, .path = FilePath(full_path)};
        res.emplace_back(fi);
      }
    }
  }
  closedir(dir);
  return res;
}

void kl::navigateTree(const Text& treeBase, std::function<NavigateInstructions(const FileInfo& file)> processor) {
  // let's make a dummy item
  FileInfo dummy{.type = FileType::Directory, .lastWrite = s_UnixEpoch, .path = FilePath(treeBase)};
  std::queue<FileInfo> to_process;
  to_process.push(dummy);
  while (!to_process.empty()) {
    FileInfo fi = to_process.front();
    to_process.pop();
    auto entries = _get_directory_entries(fi.path.fullPath());
    for (const auto& entry: entries) {
      auto res = processor(entry);
      if (res == NavigateInstructions::Continue && entry.type == FileType::Directory) {
        to_process.push(entry);
      }
      if (res == NavigateInstructions::Stop) {
        return;
      }
    }
  }
}

Text kl::getExecutablePath(const Text& exename) {
  if (!exename.contains('/')) {
    auto folders = Text(getenv("PATH")).splitByChar(':');
    for (const auto& f: folders) {
      FilePath fp(f + "/"_t + exename);
      if (fp.exists()) {
        return fp.fullPath();
      }
    }
  }
  return exename;
}

bool kl::mkDir(const Text& path) { // TODO try to not do it like a lazy individual that we all know you are.
  return std::filesystem::create_directories(path.toView());
}

bool kl::isDir(const Text& path) { // TODO try to not do it like a lazy individual that we all know you are.
  return std::filesystem::is_directory(path.toView());
}
