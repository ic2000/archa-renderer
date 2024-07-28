#pragma once

#include "config.hpp"

#include <array>

#include "barycentric_coords.hpp"
#include "colour.hpp"
#include "intrinsics.hpp"
#include "render_target.hpp"
#include "render_triangle.hpp"

namespace Archa {

class PixelProcessor {
  RenderTarget &render_target;
  const RenderTriangle &rt;

  bool is_texured{};
  glm::ivec2 texture_size{};

  int x{};

#ifdef NO_SIMD
  std::array<int, 3> w_row{};

  std::array<glm::vec2, 3> abc_t{};
#endif

#ifdef USING_SIMD_SSE2
  bool was_inside{};
  bool is_outside_right{};

  __m128 area_seq_vec{};
  __m128i bias_seq_vec{};
  __m128i w_row_seq_vec{};
  __m128i w_seq_vec{};
  __m128 clip_z_seq_vec{};
  __m128 clip_w_seq_vec{};
  __m128i delta_w_x_seq_vec{};
  __m128i delta_w_y_seq_vec{};

  __m128 abc_t_x_seq_vec{};
  __m128 abc_t_y_seq_vec{};

  std::array<__m128, 3> colours_seq_vecs{};

  __m128 area_vec{};
  __m128 texture_size_x_vec{};
  __m128 texture_size_y_vec{};

  std::array<std::array<__m128, 3>, 4> colours_vecs{};
  std::array<__m128i, 3> bias_vecs{};
  std::array<__m128, 3> clip_z_vec{};
  std::array<__m128, 3> clip_w_vec{};
  std::array<__m128i, 3> delta_w_x_init_vecs{};
  std::array<__m128i, 3> delta_w_x_step_vecs{};

  std::array<__m128, 3> abc_t_x_vecs{};
  std::array<__m128, 3> abc_t_y_vecs{};
#endif

#ifdef USING_SIMD_AVX2
  __m256 clip_w_seq_vec256{};
  __m256 abc_t_seq_vec256{};

  __m256 area_vec256{};
  __m256 texture_size_x_vec256{};
  __m256 texture_size_y_vec256{};

  std::array<std::array<__m256, 3>, 4> colours_vec256s{};
  std::array<__m256i, 3> bias_vec256s{};
  std::array<__m256, 3> clip_z_vec256s{};
  std::array<__m256, 3> clip_w_vec256s{};
  std::array<__m256i, 3> delta_w_x_init_vec256s{};
  std::array<__m256i, 3> delta_w_x_step_vec256s{};

  std::array<__m256, 3> abc_t_x_vec256s{};
  std::array<__m256, 3> abc_t_y_vec256s{};
#endif

  Colour interpolate_colour(const BarycentricCoords &bc);
  Colour interpolate_texture(const BarycentricCoords &bc);

  void process_pixel(const glm::ivec2 &pos, const BarycentricCoords &bc,
                     float z);

#ifdef NO_SIMD
  void iterate_pixels(int y);
#endif

#ifdef USING_SIMD_SSE2
  bool pixel_is_inside_mask(uint i, int mask);

  template <typename T>
  typename T::template Array<float>
  interpolate_z(const std::array<typename T::FloatVec, 3> &bc_vecs,
                     const std::array<typename T::FloatVec, 3> &clip_z_vecs) {

    std::array<typename T::FloatVec, 3> z_result_vec{};
    alignas(T::ALIGN_WIDTH) typename T::template Array<float> z_values{};

    for (uint i{0}; i < bc_vecs.size(); i++)
      z_result_vec[i] = T::multiply_floats(bc_vecs[i], clip_z_vecs[i]);

    T::store_floats(z_values.data(), T::add_floats(z_result_vec));

    return z_values;
  }

  template <typename T>
  std::array<typename T::template Array<int>, 4> interpolate_colour(
      const std::array<typename T::FloatVec, 3> &bc_vecs,
      const std::array<std::array<typename T::FloatVec, 3>, 4> &colours_vecs) {

    std::array<std::array<typename T::FloatVec, 3>, 4> colours_result_vecs{};

    alignas(T::ALIGN_WIDTH) std::array<typename T::template Array<int>, 4>
        colours{};

    for (uint c{0}; c < colours_result_vecs.size(); c++) {
      for (uint i{0}; i < colours_result_vecs[c].size(); i++)
        colours_result_vecs[c][i] =
            T::multiply_floats(colours_vecs[c][i], bc_vecs[i]);

      const auto channel_vec{
          T::convert_to_ints(T::add_floats(colours_result_vecs[c]))};

      T::store_ints(colours[c].data(), channel_vec);
    }

    return colours;
  }

  template <typename T>
  std::pair<typename T::template Array<int>, typename T::template Array<int>>
  interpolate_texture(
      const typename T::FloatVec &texture_size_x_vec,
      const typename T::FloatVec &texture_size_y_vec,
      const std::array<typename T::FloatVec, 3> &bc_vecs,
      const std::array<typename T::FloatVec, 3> &clip_w_vecs,
      const std::array<typename T::FloatVec, 3> &abc_t_x_vecs,
      const std::array<typename T::FloatVec, 3> &abc_t_y_vecs) {

    std::array<typename T::FloatVec, 3> w_abc_vecs{};
    std::array<typename T::FloatVec, 3> uv_x_abc_vecs{};
    std::array<typename T::FloatVec, 3> uv_y_abc_vecs{};

    for (uint i{0}; i < 3; i++) {
      w_abc_vecs[i] = T::divide_floats(bc_vecs[i], clip_w_vecs[i]);

      uv_x_abc_vecs[i] = T::multiply_floats(bc_vecs[i], abc_t_x_vecs[i]);
      uv_y_abc_vecs[i] = T::multiply_floats(bc_vecs[i], abc_t_y_vecs[i]);
    }

    const auto w_t_vec{T::add_floats(w_abc_vecs)};

    auto uv_x_vec{T::add_floats(uv_x_abc_vecs)};
    auto uv_y_vec{T::add_floats(uv_y_abc_vecs)};

    uv_x_vec = T::divide_floats(uv_x_vec, w_t_vec);
    uv_y_vec = T::divide_floats(uv_y_vec, w_t_vec);

    uv_x_vec =
        T::convert_to_ints(T::multiply_floats(uv_x_vec, texture_size_x_vec));

    uv_y_vec =
        T::convert_to_ints(T::multiply_floats(uv_y_vec, texture_size_y_vec));

    alignas(T::ALIGN_WIDTH) typename T::template Array<int> uv_x{};
    alignas(T::ALIGN_WIDTH) typename T::template Array<int> uv_y{};

    T::store_ints(uv_x.data(), uv_x_vec);
    T::store_ints(uv_y.data(), uv_y_vec);

    return {uv_x, uv_y};
  }

  template <typename T>
  void process_pixels(int y, int is_inside_mask,
                           const std::array<typename T::FloatVec, 3> &bc_vecs) {

    typename T::template Array<float> z_values{};

    std::array<typename T::template Array<int>, 4> colours{};

    typename T::template Array<int> uv_x{};
    typename T::template Array<int> uv_y{};

    if constexpr (std::is_same<T, AVX2>::value) {
      z_values = interpolate_z_avx2(bc_vecs);

      if (is_texured)
        std::tie(uv_x, uv_y) = interpolate_texture_avx2(bc_vecs);
      else
        colours = interpolate_colour_avx2(bc_vecs);
    }

    else {
      z_values = interpolate_z_sse2(bc_vecs);

      if (is_texured)
        std::tie(uv_x, uv_y) = interpolate_texture_sse2(bc_vecs);
      else
        colours = interpolate_colour_sse2(bc_vecs);
    }

    for (uint i{0}; i < T::LANE_WIDTH; i++) {
      if (pixel_is_inside_mask(i, is_inside_mask)) {
        was_inside = true;

        const glm::ivec2 pos{x + static_cast<int>(i), y};

        if (z_values[i] >= render_target.z_buffer.get(pos))
          continue;

        render_target.z_buffer.set(pos, z_values[i]);

        Colour colour{};

        if (is_texured)
          colour = rt.triangle.diffuse_texture->get_pixel({uv_x[i], uv_y[i]});

        else
          colour = {static_cast<uint8>(colours[0][i]),
                    static_cast<uint8>(colours[1][i]),
                    static_cast<uint8>(colours[2][i]),
                    static_cast<uint8>(colours[3][i])};

        render_target.frame_buffer.set_pixel(pos, colour);
      }

      else if (was_inside) {
        is_outside_right = true;

        x = rt.box.max.x;
      }
    }
  }

  template <typename T>
  void iterate_pixels(
      int y, typename T::FloatVec &area_vec,
      const std::array<typename T::IntVec, 3> &bias_vecs,
      const std::array<typename T::IntVec, 3> &delta_w_x_init_vecs,
      const std::array<typename T::IntVec, 3> &delta_w_x_step_vecs) {

    std::array<typename T::IntVec, 3> w_vecs{
        T::set_int(SSE2::extract_int<0>(w_seq_vec)),
        T::set_int(SSE2::extract_int<1>(w_seq_vec)),
        T::set_int(SSE2::extract_int<2>(w_seq_vec))};

    for (uint i{0}; i < w_vecs.size(); i++)
      w_vecs[i] = T::add_ints(w_vecs[i], delta_w_x_init_vecs[i]);

    for (; x < rt.box.max.x - (T::LANE_WIDTH - 1); x += T::LANE_WIDTH) {
      std::array<typename T::IntVec, 3> w_with_bias_vecs{};

      for (uint i{0}; i < w_vecs.size(); i++)
        w_with_bias_vecs[i] = T::add_ints(w_vecs[i], bias_vecs[i]);

      const auto is_inside_vec{
          T::compare_ints_gt(T::or_ints(w_with_bias_vecs), T::minus_one_ints)};

      const auto is_inside_mask{T::move_mask_int8(is_inside_vec)};

      std::array<typename T::FloatVec, 3> bc_vecs{};

      for (uint i{0}; i < bc_vecs.size(); i++)
        bc_vecs[i] =
            T::divide_floats(T::convert_to_floats(w_vecs[i]), area_vec);

      process_pixels<T>(y, is_inside_mask, bc_vecs);

      for (uint i{0}; !is_outside_right & (i < w_vecs.size()); i++)
        w_vecs[i] = T::add_ints(w_vecs[i], delta_w_x_step_vecs[i]);
    }

    w_seq_vec = SSE2::set_ints(0, T::template extract_int<0>(w_vecs[2]),
                               T::template extract_int<0>(w_vecs[1]),
                               T::template extract_int<0>(w_vecs[0]));
  }

  SSE2::Array<float> interpolate_z_sse2(const std::array<__m128, 3> &bc_vecs);

  std::array<SSE2::Array<int>, 4>
  interpolate_colour_sse2(const std::array<__m128, 3> &bc_vecs);

  std::pair<SSE2::Array<int>, SSE2::Array<int>>
  interpolate_texture_sse2(const std::array<__m128, 3> &bc_vecs);

  void process_pixels_sse2(int y, int is_inside_mask,
                           const std::array<__m128, 3> &bc_vecs);

  void iterate_pixels_sse2(int y);
  void iterate_pixels_sequentially_sse2(int y);
#endif

#ifdef USING_SIMD_AVX2
  AVX2::Array<float>
  interpolate_z_avx2(const std::array<__m256, 3> &bc_vec256s);

  std::array<AVX2::Array<int>, 4>
  interpolate_colour_avx2(const std::array<__m256, 3> &bc_vec256s);

  std::pair<AVX2::Array<int>, AVX2::Array<int>>
  interpolate_texture_avx2(const std::array<__m256, 3> &bc_vec256s);

  void process_pixels_avx2(int y, int is_inside_mask,
                           const std::array<__m256, 3> &bc_vec256s);

  void iterate_pixels_avx2(int y);
#endif

public:
  PixelProcessor(RenderTarget &render_target, const RenderTriangle &rt);

  void iterate_x(int y);
  void step_y();
};

} // namespace Archa
