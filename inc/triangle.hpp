#pragma once

#include "config.hpp"

#include <SFML/Graphics/Image.hpp>
#include <array>
#include <glm/glm.hpp>

#include "image.hpp"
#include "types.hpp"

namespace Archa {

struct Triangle {
  std::array<uint, 3> i{};
  std::array<glm::vec2, 3> uvs{};
  std::array<glm::vec3, 3> normals{};
  std::shared_ptr<const Image> diffuse_texture{};
};

} // namespace Archa
