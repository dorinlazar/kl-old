#pragma once

// Markwrite file format (.mw), format definition in progress.

#include "kl/klds.h"
#include "kl/kltext.h"
#include <variant>

namespace kl::mw {
using namespace kl;

struct Link {};
struct Footnote {};

struct Paragraph {};

struct ListItem {};

struct ListContainer {
  List<ListItem> _items;
};

using Block = std::variant<Paragraph, ListContainer>;

struct Document {
  List<Block> _blocks;
  List<Link> _links;
  List<Footnote> _footnotes;
};
} // namespace kl::mw
