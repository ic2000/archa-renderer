#include "rasteriser.hpp"

#include <thread>

#include "bounding_box.hpp"
#include "colour.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "error.hpp"
#include "frame_buffer.hpp"
#include "intrinsics.hpp"
#include "logger.hpp"
#include "pixel_processor.hpp"
#include "render_triangle.hpp"
#include "types.hpp"
#include "util.hpp"
#include "z_buffer.hpp"

namespace Archa {

void Rasteriser::compute_projection_transform() {
  const auto &size{render_target.size};

  const auto height_over_width{static_cast<float>(size.y) /
                               static_cast<float>(size.x)};

  const auto tan_half_fov{glm::tan(glm::radians(camera->get_fov() / 2.0f))};
  const auto z_range{camera->get_z_near() - camera->get_z_far()};

  projection_transform[0][0] = 1.0f / tan_half_fov * height_over_width;
  projection_transform[1][1] = 1.0f / tan_half_fov;

  projection_transform[2][2] =
      (-camera->get_z_near() - camera->get_z_far()) / z_range;

  projection_transform[2][3] = 1.0f;

  projection_transform[3][2] =
      2 * camera->get_z_far() * camera->get_z_near() / z_range;
}

void Rasteriser::compute_screen_space_transform() {
  const auto &size{render_target.size};

  const auto &half_width{static_cast<float>(size.x) / 2.0f};
  const auto &half_height{static_cast<float>(size.y) / 2.0f};

  screen_space_transform[0][0] = half_width;
  screen_space_transform[1][1] = -half_height;
  screen_space_transform[3][0] = half_width;
  screen_space_transform[3][1] = half_height;
}

void Rasteriser::clear_bin(const Bin &bin) {
  auto &frame_buffer{render_target.frame_buffer};
  auto &z_buffer{render_target.z_buffer};

  const auto &bin_min{bin.get_pos()};
  const auto &bin_max{bin_min + bin.get_size()};

  const auto &fill_colour{bin.get_fill_colour()};

  for (int y{bin_min.y}; y < bin_max.y; y++) {
    auto x{bin_min.x};
    auto z_buffer_index{y * render_target.size.x + x};

#ifdef USING_SIMD_AVX2
    for (; x < bin_max.x - (AVX2::LANE_WIDTH - 1);
         x += AVX2::LANE_WIDTH, z_buffer_index += AVX2::LANE_WIDTH) {

      ALIGN_AVX2 const AVX2::Array<Colour> colours{
          fill_colour, fill_colour, fill_colour, fill_colour,
          fill_colour, fill_colour, fill_colour, fill_colour};

      frame_buffer.set_pixels({x, y}, colours);
      z_buffer.clear_lane_avx2(z_buffer_index);
    }
#endif

#ifdef USING_SIMD_SSE2
    for (; x < bin_max.x - (SSE2::LANE_WIDTH - 1);
         x += SSE2::LANE_WIDTH, z_buffer_index += SSE2::LANE_WIDTH) {
      ALIGN_SSE2 const SSE2::Array<Colour> colours{fill_colour, fill_colour,
                                                   fill_colour, fill_colour};

      frame_buffer.set_pixels({x, y}, colours);
      z_buffer.clear_lane_sse2(z_buffer_index);
    }
#endif

    for (; x < bin_max.x; x++, z_buffer_index++) {
      frame_buffer.set_pixel({x, y}, fill_colour);
      z_buffer.clear_single(z_buffer_index);
    }
  }
}

void Rasteriser::resize_bins(int bin_count) {
  binner.split_bins(render_target.size, bin_count);
  futures.resize(binner.get_bins().size());
}

void Rasteriser::create(const glm::ivec2 &size, int bin_count) {
  render_target.create(size);
  resize_bins(bin_count);

  if (camera)
    compute_projection_transform();

  compute_screen_space_transform();
}

void Rasteriser::set_camera(const Camera *camera) {
  this->camera = camera;

  compute_projection_transform();
}

static const std::vector<std::pair<uint, BoundingBox>> &
split_bounding_box(const BoundingBox &box, const std::vector<Bin> &bins,
                   const glm::ivec2 &screen_size) {

  thread_local auto result{create_reserved_vector<std::pair<uint, BoundingBox>>(
      std::thread::hardware_concurrency())};

  result.clear();

  for (uint i{0}; i < bins.size(); i++) {
    const auto &bin{bins[i]};

    const auto &bin_min{glm::max({0, 0}, bins[i].get_pos())};
    const auto &bin_max{glm::min(screen_size, bin_min + bin.get_size())};

    if (!box.overlaps(bin_min, bin_max))
      continue;

    const auto &min{glm::max(box.min, bin_min)};
    const auto &max{glm::min(box.max, bin_max)};

    result.emplace_back(i, BoundingBox{min, max});
  }

  return result;
}

#ifdef USING_SIMD_AVX2
void Rasteriser::iterate_boxes_avx2(
    const BoundingBox &box,
    const std::vector<std::pair<uint, BoundingBox>> &boxes,
    const std::array<int, 3> &w_row, const std::array<glm::ivec2, 3> &delta_w,
    uint &i, RenderTriangle &render_triangle) {

  std::array<__m256i, 3> w_row_vec256{};
  std::array<__m256i, 3> delta_w_vec256{};

  const auto boxes_per_avx2_lane{AVX2::LANE_WIDTH / 2};

  if (boxes.size() >= boxes_per_avx2_lane) {
    for (uint i{0}; i < 3; i++) {
      w_row_vec256[i] =
          AVX2::set_ints(w_row[i], w_row[i], w_row[i], w_row[i], 0, 0, 0, 0);

      delta_w_vec256[i] = AVX2::set_ints(
          delta_w[i].x, delta_w[i].x, delta_w[i].x, delta_w[i].x, delta_w[i].y,
          delta_w[i].y, delta_w[i].y, delta_w[i].y);
    }
  }

  for (; static_cast<int>(i) < std::ssize(boxes) - (boxes_per_avx2_lane - 1);
       i += boxes_per_avx2_lane) {
    __m256i box_min_vec256{AVX2::set_ints(box.min.x, box.min.x, box.min.x,
                                          box.min.x, box.min.y, box.min.y,
                                          box.min.y, box.min.y)};

    __m256i boxes_min_vec256{
        AVX2::set_ints(boxes[i + 3].second.min.x, boxes[i + 2].second.min.x,
                       boxes[i + 1].second.min.x, boxes[i].second.min.x,
                       boxes[i + 3].second.min.y, boxes[i + 2].second.min.y,
                       boxes[i + 1].second.min.y, boxes[i].second.min.y)};

    __m256i box_difference_vec256{
        AVX2::subtract_ints(boxes_min_vec256, box_min_vec256)};

    ALIGN_SSE2 std::array<std::array<int, 4>, 3> new_w_row{};

    for (uint j{0}; j < 3; j++) {
      const auto &new_w_row_vec256{AVX2::add_ints(
          w_row_vec256[j],
          AVX2::multiply_ints(delta_w_vec256[j], box_difference_vec256))};

      const auto &new_w_row_vec{
          SSE2::add_ints(AVX2::extract_ints128<0>(new_w_row_vec256),
                         AVX2::extract_ints128<1>(new_w_row_vec256))};

      SSE2::store_ints(new_w_row[j].data(), new_w_row_vec);
    }

    for (uint j{0}; j < boxes_per_avx2_lane; j++) {
      const auto box_index{i + j};

      const std::array<int, 3> w_row_new_j{new_w_row[0][j], new_w_row[1][j],
                                           new_w_row[2][j]};

      render_triangle.box = boxes[box_index].second;
      render_triangle.w_row = w_row_new_j;

      binner.get_render_bin_group(boxes[box_index].first)
          .push_back(render_triangle);
    }
  }
}
#endif

void Rasteriser::iterate_boxes(
    const BoundingBox &box,
    const std::vector<std::pair<uint, BoundingBox>> &boxes,
    const std::array<int, 3> &w_row, const std::array<glm::ivec2, 3> &delta_w,
    uint i, RenderTriangle &render_triangle) {

  for (; i < boxes.size(); i++) {
    const auto &[bin_index, bin_box]{boxes[i]};
    const auto &box_difference{bin_box.min - box.min};

    std::array<int, 3> w_row_new{};

    for (uint i{0}; i < w_row_new.size(); i++) {
      const auto &new_delta_w{delta_w[i] * box_difference};

      w_row_new[i] = w_row[i] + new_delta_w.y + new_delta_w.x;
    }

    render_triangle.box = bin_box;
    render_triangle.w_row = w_row_new;

    binner.get_render_bin_group(bin_index).push_back(render_triangle);
  }
}

static int edge_cross(const glm::ivec2 &a, const glm::ivec2 &b,
                      const glm::ivec2 &p) {
  const glm::ivec2 &ab{b - a};
  const glm::ivec2 &ap{p - a};

  return ab.x * ap.y - ab.y * ap.x;
}

static bool is_top_left(const glm::ivec2 &start, const glm::ivec2 &end) {
  const auto edge{end - start};

  bool is_top_edge{edge.y == 0 && edge.x > 0};
  bool is_left_edge{edge.y < 0};

  return is_top_edge || is_left_edge;
}

void Rasteriser::process_triangle(const std::vector<Vertex> &vertices,
                                  Triangle triangle,
                                  const glm::mat4 &transform) {

  const auto vp{projection_transform * transform};

  const auto &v0{vertices[triangle.i[0]]};
  const auto &v1{vertices[triangle.i[1]]};
  const auto &v2{vertices[triangle.i[2]]};

  // for (auto &normal : triangle.normals)
  // normal = glm::normalize(glm::vec3{transform * glm::vec4{normal, 0.0f}});

  const std::array<glm::vec4, 3> clip{vp * glm::vec4{v0, 1.0f},
                                      vp * glm::vec4{v1, 1.0f},
                                      vp * glm::vec4{v2, 1.0f}};

  const std::array<Colour, 3> colours{v0.colour, v1.colour, v2.colour};

  triangle.uvs = {triangle.uvs[0], triangle.uvs[1], triangle.uvs[2]};

  triangle.normals = {triangle.normals[0], triangle.normals[1],
                      triangle.normals[2]};

  const std::array<glm::vec4, 3> screen{screen_space_transform * clip[0],
                                        screen_space_transform * clip[1],
                                        screen_space_transform * clip[2]};

  const std::array<glm::ivec2, 3> v{glm::ivec2{screen[0] / screen[0].w},
                                    glm::ivec2{screen[1] / screen[1].w},
                                    glm::ivec2{screen[2] / screen[2].w}};

  const auto &area{edge_cross(v[0], v[1], v[2])};

  if (area <= 0)
    return;

  const auto &box{BoundingBox::from_points(v[0], v[1], v[2])};

  if (!box.overlaps({0, 0}, render_target.size))
    return;

  const auto &boxes{
      split_bounding_box(box, binner.get_bins(), render_target.size)};

  if (boxes.empty())
    return;

  const auto &p0{box.min};

  const std::array<int, 3> w_row{edge_cross(v[1], v[2], p0),
                                 edge_cross(v[2], v[0], p0),
                                 edge_cross(v[0], v[1], p0)};

  const std::array<glm::ivec2, 3> delta_w{
      glm::ivec2(v[1].y - v[2].y, v[2].x - v[1].x),
      glm::ivec2(v[2].y - v[0].y, v[0].x - v[2].x),
      glm::ivec2(v[0].y - v[1].y, v[1].x - v[0].x)};

  const std::array<int8, 3> bias{
      static_cast<int8>(is_top_left(v[1], v[2]) ? 0 : -1),
      static_cast<int8>(is_top_left(v[2], v[0]) ? 0 : -1),
      static_cast<int8>(is_top_left(v[0], v[1]) ? 0 : -1)};

  RenderTriangle render_triangle{.triangle = triangle,
                                 .colours = colours,
                                 .area = area,
                                 .bias = bias,
                                 .clip = clip,
                                 .delta_w = delta_w};

  uint i{0};

#ifdef USING_SIMD_AVX2
  iterate_boxes_avx2(box, boxes, w_row, delta_w, i, render_triangle);
#endif

  iterate_boxes(box, boxes, w_row, delta_w, i, render_triangle);
}

void Rasteriser::render_triangle(RenderTriangle &rt) {
  PixelProcessor pixel_processor{render_target, rt};

  for (auto y{rt.box.min.y}; y < rt.box.max.y; y++) {
    pixel_processor.iterate_x(y);
    pixel_processor.step_y();
  }
}

void Rasteriser::render_scene(const Scene &scene,
                              BS::thread_pool &thread_pool) {
  futures.clear();

  for (uint i{0}; i < binner.get_bins().size(); i++)
    futures.push_back(thread_pool.submit_task([this, i] {
      clear_bin(binner.get_bins()[i]);
      binner.get_render_bin_group(i).clear();
    }));

  for (auto &future : futures)
    future.get();

  futures.clear();

  for (const auto &model_instance : scene.model_instances) {
    const auto &model{model_instance.model};

    for (const auto &triangle : model.triangles)
      process_triangle(model.vertices, triangle,
                       model_instance.get_transform());
  }

  for (uint i{0}; i < binner.get_bins().size(); i++)
    futures.push_back(thread_pool.submit_task([this, i] {
      for (auto &rt : binner.get_render_bin_group(i))
        render_triangle(rt);
    }));

  for (auto &future : futures)
    future.get();
}

const sf::Texture &Rasteriser::get_texture() const {
  return render_target.blit();
}

const glm::ivec2 &Rasteriser::get_size() const { return render_target.size; }

} // namespace Archa
