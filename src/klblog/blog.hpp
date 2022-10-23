#pragma once
#include "systemsettings.hpp"
#include "blogsettings.hpp"

namespace klblog {

class Blog {
  Settings settings;

public:
  Blog(std::shared_ptr<SystemSettings> sys);
  void process() const;
};

} // namespace klblog