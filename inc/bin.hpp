#pragma once

#include "config.hpp"

#include <glm/glm.hpp>

#include "colour.hpp"

namespace Archa {

class Bin {

  glm::ivec2 pos{};
  glm::ivec2 size{};

  Colour fill_colour{};

public:
  void create(const glm::ivec2 &pos, const glm::ivec2 &size,
              const Colour &fill_colour = Colour::Black);

  const glm::ivec2 &get_pos() const;
  const glm::ivec2 &get_size() const;

  const Colour &get_fill_colour() const;
};

} // namespace Archa
