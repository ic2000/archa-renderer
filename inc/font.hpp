#pragma once

#include "config.hpp"

#include <imgui.h>
#include <vector>

#include "resource.hpp"
#include "types.hpp"

namespace Archa {
class Font : public Resource {
  mutable std::vector<uint8> font_data{};

public:
  void load(const std::filesystem::path &file_path) override;

  ImFont *get_font(float size) const;
};

} // namespace Archa
