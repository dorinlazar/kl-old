#include "markwrite.h"
#include "poorconfig.h"
using namespace kl;

Document::Document(const Text& content) {
  TextScanner scanner(content);
  _metadata.update(scanner);
  _content.update(scanner);
}

void DocumentContent::update(TextScanner& scanner) { _baseContent = scanner.remainder(); }
void DocumentMetadata::update(TextScanner& scanner) {
  _properties = PoorConfig::parse(scanner);
  CHECK(_properties->isMap(), "Expected a map as document metadata");
  auto title = _properties->getOpt("title");
  if (title.has_value()) {
    _title = title.value();
  }
  auto pubTime = _properties->getOpt("date");
  if (pubTime.has_value()) {
    _publish = DateTime::parse(*pubTime);
  }
  _lastUpdate = _publish;
  pubTime = _properties->getOpt("updated");
  if (pubTime.has_value()) {
    _lastUpdate = DateTime::parse(*pubTime);
  }
}
