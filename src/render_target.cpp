#include "render_target.hpp"

namespace Archa {

void RenderTarget::create(const glm::ivec2 &size) {
  this->size = size;

  z_buffer.create(size);
  frame_buffer.create(size);
  texture.create(static_cast<uint>(size.x), static_cast<uint>(size.y));
}

const sf::Texture &RenderTarget::blit() const {
  texture.update(frame_buffer.get_pixels());

  return texture;
}

} // namespace Archa
