#pragma once
#include "kl/kltext.h"

namespace klblog {
struct Settings {
public:
  void parse(const kl::Text& filename);
  void log_abstract() const;

public:
  kl::Text website_url;      // url: https://example.com
  kl::Text website_title;    // title: Example blog
  kl::Text copyright_notice; // copyright: "© 2022 Author Name. All rights reserved."

  kl::Text default_language; // language: ro
  kl::Text theme_name;       // theme: theme_folder_name
};
} // namespace klblog
