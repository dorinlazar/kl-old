#pragma once

// Markwrite file format (.mw), format definition in progress.

#include "kl/klds.h"
#include "kl/kltext.h"
#include "kl/kltime.h"
#include <variant>

namespace kl::mw {
using namespace kl;

struct ContentBlock {};

struct Url {
  Text _url;
};

struct Author {
  Text _name;
  Url _link;
};

struct Link {
  Url _url;
  Text _hoverText;
  Text _displayText;
};

struct Citation {};
struct Footnote {};

struct Paragraph : public ContentBlock {};

struct ListView : public ContentBlock {};

struct DocumentMetadata {
  Text _title;
  DateTime _publish;
  DateTime _lastUpdate;
  PList<Author> _authors;
  Url _featuredImage;
};

struct Chapter {
  Text _title;
  Text _shortname;
  PList<ContentBlock> _content;
  PList<Chapter> _subchapters;
};

struct Document {
  DocumentMetadata _metadata;
  PList<ContentBlock> _preamble;
  PList<Chapter> _chapters;
  PList<Link> _links;
  PList<Citation> _citations;
  PList<Footnote> _footnotes;
};
} // namespace kl::mw
