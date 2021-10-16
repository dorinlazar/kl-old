#pragma once

// Markwrite file format (.mw), format definition in progress.

#include "kl/klds.h"
#include "kl/kltext.h"
#include "kl/kltime.h"
#include "kl/klvalue.h"
#include "ff/textscanner.h"
#include <variant>

namespace kl {

struct DocumentMetadata {
  Text _title;
  DateTime _publish;
  DateTime _lastUpdate;
  List<Text> _authors;
  Text _featuredImage;
  PValue _properties;

public:
  void read(TextScanner& scanner);
};

struct DocumentContent {
  Text _baseContent;

  void _readParagraph();

public:
  void read(TextScanner& scanner);
};

struct Document {
  DocumentMetadata _metadata;
  DocumentContent _content;

public:
  Document(const Text& content);
};

} // namespace kl
