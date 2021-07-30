#pragma once

#include "modules.h"

class Toolchain {
public:
  virtual bool build(const kl::Text& source, const kl::Text& destination, const kl::List<kl::Text>& include) = 0;
  virtual bool link(const kl::List<kl::Text>& objects, const kl::Text& executable,
                    const kl::List<kl::Text>& libraries) = 0;
};

class Gcc : public Toolchain {
  kl::Text _cppStandard = "-std=c++20";
  kl::Text _cStandard = "-std=c18";
  bool _staticBuild = true;

  kl::List<kl::Text> _cCompileBase;
  kl::List<kl::Text> _cppCompileBase;
  kl::List<kl::Text> _linkBase;

public:
  Gcc();
  bool build(const kl::Text& source, const kl::Text& destination, const kl::List<kl::Text>& include) override;
  bool link(const kl::List<kl::Text>& objects, const kl::Text& executable,
            const kl::List<kl::Text>& libraries) override;
};
