#pragma once

#include "config.hpp"

#include <glm/glm.hpp>

#include "colour.hpp"

namespace Archa {

struct Vertex : public glm::vec3 {
  Colour colour{};

  Vertex(const glm::vec3 &position, const Colour &colour = Colour::White)
      : glm::vec3{position}, colour{colour} {}
};

} // namespace Archa
