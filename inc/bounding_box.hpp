#pragma once

#include "config.hpp"

#include <glm/glm.hpp>

namespace Archa {

struct BoundingBox {
  glm::ivec2 min{}, max{};

  static BoundingBox from_points(const glm::ivec2 &p0, const glm::ivec2 &p1,
                                 const glm::ivec2 &p2);

  bool overlaps(const glm::ivec2 &bound_min, const glm::ivec2 &bound_max) const;
};

} // namespace Archa
