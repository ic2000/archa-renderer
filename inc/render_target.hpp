#pragma once

#include "config.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <glm/glm.hpp>

#include "frame_buffer.hpp"
#include "z_buffer.hpp"

namespace Archa {

struct RenderTarget {
  glm::ivec2 size{};

  FrameBuffer frame_buffer{};
  ZBuffer z_buffer{};

  void create(const glm::ivec2 &size);

  const sf::Texture &blit() const;

private:
  mutable sf::Texture texture{};
};

} // namespace Archa
