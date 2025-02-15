#include "viewport.hpp"

#include "error.hpp"
#include "logger.hpp"

namespace Archa {

void Viewport::create(glm::ivec2 size, const uint threads) {
  if (!camera)
    Logger().warn() << "Camera not set upon Viewport creation" << '\n';

  thread_pool = std::make_unique<BS::thread_pool>(threads);
  futures.reserve(threads);

  size = glm::max(size, {1, 1}); // ensure valid texture size

  Logger().info() << "Creating viewport of resolution " << size.x << "x"
                  << size.y << '\n';

  if (size.x <= 0 || size.y <= 0)
    fatal_error("Invalid viewport resolution");

  if (size.x % SIMD_ALIGN_WIDTH != 0)
    Logger().warn() << "Viewport width (" << size.x
                    << " px) not aligned to SIMD width (" << SIMD_ALIGN_WIDTH
                    << " px)" << '\n';

  rasteriser.create(size, static_cast<int>(threads));
}

void Viewport::set_camera(const Camera &camera) {
  this->camera = &camera;

  rasteriser.set_camera(this->camera);
}

void Viewport::set_scene(const Scene &scene) { this->scene = &scene; }

void Viewport::render() { rasteriser.render_scene(*scene, *thread_pool); }

const sf::Texture &Viewport::get_texture() const {
  return rasteriser.get_texture();
}

}; // namespace Archa
