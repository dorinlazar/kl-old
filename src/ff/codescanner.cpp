#include "codescanner.h"
#include "textscanner.h"
#include "kl/klfs.h"
#include <optional>

using namespace kl;

struct SourceCodeScannerImpl {
  TextScanner _scanner;
  Text _filename;
  List<Text> _local;
  List<Text> _system;
  bool _hasMain = false;

  SourceCodeScannerImpl(const Text& file) : _scanner(kl::FileReader(file).readAll()), _filename(file) { _scanFile(); }

  void _scanFile() {
    _scanner.skipWhitespace();
    while (!_scanner.empty()) {
      if (_scanner.topChar() == '#') {
        readInclude();
      } else if (checkForMain()) {
        _hasMain = true;
      }
      _scanner.readLine();
      _scanner.skipWhitespace();
    }
  }

  void readInclude() {
    _scanner.expect('#');
    _scanner.skipWhitespace();
    if (_scanner.readWord() == "include"_t) {
      _scanner.skipWhitespace();
      if (_scanner.topChar() == '"') {
        _local.add(_scanner.readQuotedString());
      } else if (_scanner.topChar() == '<') {
        _system.add(_scanner.readUntil('>'));
      }
    }
  }

  bool checkForMain() {
    if (_scanner.readWord() == "int") {
      _scanner.skipWhitespace();
      return _scanner.readWord() == "main";
    }
    return false;
  }
};

SourceCodeScanner::SourceCodeScanner(const kl::Text& filename) {
  SourceCodeScannerImpl scanner(filename);
  _local = scanner._local;
  _system = scanner._system;
  _hasMain = scanner._hasMain;
}

const List<Text>& SourceCodeScanner::localIncludes() { return _local; }
const List<Text>& SourceCodeScanner::systemIncludes() { return _system; }
bool SourceCodeScanner::hasMain() { return _hasMain; }
