#pragma once

#include "config.hpp"

#include <filesystem>

namespace Archa {

class Resource {
public:
  virtual ~Resource() = default;
  virtual void load(const std::filesystem::path &file_path) = 0;
};

} // namespace Archa
