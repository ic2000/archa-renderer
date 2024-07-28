#pragma once

#include "config.hpp"

#include <vector>

#include "bin.hpp"
#include "render_triangle.hpp"

namespace Archa {

class Binner {
  std::vector<Bin> bins{};
  RenderTriangleGroups render_triangle_bin_groups{};

public:
  void split_bins(const glm::ivec2 &size, int count);

  const std::vector<Bin> &get_bins() const;

  std::vector<RenderTriangle>& get_render_bin_group(uint index);
};

} // namespace Archa
