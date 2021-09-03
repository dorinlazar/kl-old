#pragma once

#include "kl/kltext.h"
#include "kl/klds.h"

class Toolchain {
public:
  virtual bool build(const kl::Text& source, const kl::Text& destination, const kl::List<kl::Text>& include) = 0;
  virtual bool link(const kl::List<kl::Text>& objects, const kl::Text& executable,
                    const kl::List<kl::Text>& libraries) = 0;
  virtual kl::List<kl::Text> buildCmdLine(const kl::Text& source, const kl::Text& destination,
                                          const kl::List<kl::Text>& include) = 0;
  virtual kl::List<kl::Text> linkCmdLine(const kl::List<kl::Text>& objects, const kl::Text& executable,
                                         const kl::List<kl::Text>& libraries) = 0;
};

class Gcc : public Toolchain {
  bool _staticBuild = true;

  kl::List<kl::Text> _cCompileBase;
  kl::List<kl::Text> _cppCompileBase;
  kl::List<kl::Text> _linkBase;

public:
  Gcc();
  bool build(const kl::Text& source, const kl::Text& destination, const kl::List<kl::Text>& include) override;
  bool link(const kl::List<kl::Text>& objects, const kl::Text& executable,
            const kl::List<kl::Text>& libraries) override;
  kl::List<kl::Text> buildCmdLine(const kl::Text& source, const kl::Text& destination,
                                  const kl::List<kl::Text>& include) override;
  kl::List<kl::Text> linkCmdLine(const kl::List<kl::Text>& objects, const kl::Text& executable,
                                 const kl::List<kl::Text>& libraries) override;
};
