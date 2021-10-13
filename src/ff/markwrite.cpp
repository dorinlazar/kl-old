#include "markwrite.h"
#include "poorconfig.h"
using namespace kl;

Document::Document(const Text& content) {
  TextScanner scanner(content);
  _metadata.read(scanner);
  _content.read(scanner);
}

void DocumentMetadata::read(TextScanner& scanner) {
  static DateTime defaultDate(2020, 1, 1);
  _properties = PoorConfig::parse(scanner);
  CHECK(_properties->isMap(), "Expected a map as document metadata");
  _title = _properties->getOpt("title").value_or(""_t);
  _featuredImage = _properties->getOpt("image").value_or(""_t);

  auto pubTime = _properties->getOpt("date");
  _publish = pubTime.has_value() ? DateTime::parse(*pubTime) : defaultDate;

  pubTime = _properties->getOpt("updated");
  _lastUpdate = pubTime.has_value() ? DateTime::parse(*pubTime) : _publish;

  auto m = _properties->asMap();
  if (_properties->asMap().has("author")) {
    _authors = _properties->get("author")->getArrayValue();
  }
}

void DocumentContent::_readParagraph() {}

void DocumentContent::read(TextScanner& scanner) {
  _baseContent = scanner.remainder();
  while (!scanner.empty()) {
    _readParagraph();
  }
}
