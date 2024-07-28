#pragma once

#include "config.hpp"

#include <BS_thread_pool.hpp>
#include <SFML/Graphics.hpp>
#include <future>
#include <glm/glm.hpp>

#include "SFML/Graphics/Texture.hpp"
#include "camera.hpp"
#include "rasteriser.hpp"
#include "scene.hpp"
#include "types.hpp"

namespace Archa {

class Viewport {
  std::unique_ptr<BS::thread_pool> thread_pool{};
  std::vector<std::future<void>> futures{};

  const Camera *camera{nullptr};
  const Scene *scene{nullptr};

  Rasteriser rasteriser{};

public:
  void create(glm::ivec2 size, const uint threads);

  void set_camera(const Camera &camera);
  void set_scene(const Scene &scene);

  void render();

  const sf::Texture &get_texture() const;
};

} // namespace Archa
