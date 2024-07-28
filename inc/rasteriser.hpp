#pragma once

#include "config.hpp"

#include <BS_thread_pool.hpp>
#include <future>
#include <glm/glm.hpp>

#include "bin.hpp"
#include "binner.hpp"
#include "camera.hpp"
#include "render_target.hpp"
#include "render_triangle.hpp"
#include "scene.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace Archa {

class Rasteriser {
  RenderTarget render_target{};
  Binner binner{};

  const Camera *camera{nullptr};

  glm::mat4 projection_transform{0};
  glm::mat4 screen_space_transform{1};

  std::vector<std::future<void>> futures{};

  void compute_projection_transform();
  void compute_screen_space_transform();

  void clear_bin(const Bin &bin);

public:
  void resize_bins(int bin_count);

  void create(const glm::ivec2 &size, int bin_count);

  void set_camera(const Camera *camera);

#ifdef USING_SIMD_AVX2
  void
  iterate_boxes_avx2(const BoundingBox &box,
                     const std::vector<std::pair<uint, BoundingBox>> &boxes,
                     const std::array<int, 3> &w_row,
                     const std::array<glm::ivec2, 3> &delta_w, uint &i,
                     RenderTriangle &render_triangle);
#endif

  void iterate_boxes(const BoundingBox &box,
                     const std::vector<std::pair<uint, BoundingBox>> &boxes,
                     const std::array<int, 3> &w_row,
                     const std::array<glm::ivec2, 3> &delta_w, uint i,
                     RenderTriangle &render_triangle);

  void process_triangle(const std::vector<Vertex> &vertices, Triangle triangle,
                        const glm::mat4 &transform);

  void render_triangle(RenderTriangle &rt);
  void render_scene(const Scene &scene, BS::thread_pool &thread_pool);

  const sf::Texture &get_texture() const;
  const glm::ivec2 &get_size() const;
};

} // namespace Archa
