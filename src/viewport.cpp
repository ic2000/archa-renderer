#include "viewport.hpp"

#include "logger.hpp"

namespace Archa {

void Viewport::create(glm::ivec2 size, const uint threads) {
  if (!camera)
    Logger().warn() << "Camera not set upon Viewport creation" << std::endl;

  size = glm::max(size, {1, 1}); // ensure valid texture size

  thread_pool = std::make_unique<BS::thread_pool>(threads);
  futures.reserve(threads);

  rasteriser.create(size, static_cast<int>(thread_pool->get_thread_count()));
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
