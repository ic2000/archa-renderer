#include "bounding_box.hpp"

namespace Archa {

BoundingBox BoundingBox::from_points(const glm::ivec2 &p0, const glm::ivec2 &p1,
                                     const glm::ivec2 &p2) {
  BoundingBox box{};

  box.min = glm::min(p0, glm::min(p1, p2));
  box.max = glm::max(p0, glm::max(p1, p2));

  return box;
}

bool BoundingBox::overlaps(const glm::ivec2 &bound_min,
                           const glm::ivec2 &bound_max) const {
  return (min.x <= bound_max.x && max.x >= bound_min.x) &&
         (min.y <= bound_max.y && max.y >= bound_min.y);
}

} // namespace Archa
