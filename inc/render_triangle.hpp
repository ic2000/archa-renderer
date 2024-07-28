#pragma once

#include "config.hpp"

#include <array>

#include "bounding_box.hpp"
#include "colour.hpp"
#include "triangle.hpp"

namespace Archa {

struct RenderTriangle {
  Triangle triangle{};
  std::array<Colour, 3> colours{};
  BoundingBox box{};
  int area{};
  std::array<int8, 3> bias{};
  std::array<int, 3> w_row{};
  std::array<glm::vec4, 3> clip{};
  std::array<glm::ivec2, 3> delta_w{};
};

using RenderTriangleGroups = std::vector<std::vector<RenderTriangle>>;

} // namespace Archa
