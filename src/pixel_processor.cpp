#include "pixel_processor.hpp"

#include "colour.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "image.hpp"
#include "intrinsics.hpp"

namespace Archa {

Colour PixelProcessor::interpolate_colour(const BarycentricCoords &bc) {
#ifdef NO_SIMD
  return {
      static_cast<uint8>(rt.colours[0].r * bc.a + rt.colours[1].r * bc.b +
                         rt.colours[2].r * bc.g),

      static_cast<uint8>(rt.colours[0].g * bc.a + rt.colours[1].g * bc.b +
                         rt.colours[2].g * bc.g),

      static_cast<uint8>(rt.colours[0].b * bc.a + rt.colours[1].b * bc.b +
                         rt.colours[2].b * bc.g),

      static_cast<uint8>(rt.colours[0].a * bc.a + rt.colours[1].a * bc.b +
                         rt.colours[2].a * bc.g),
  };
#endif

#ifdef USING_SIMD_SSE2
  const std::array<__m128, 3> bc_component_vecs{
      SSE2::set_float(bc.a), SSE2::set_float(bc.b), SSE2::set_float(bc.g)};

  std::array<__m128, 3> colours_result_vecs{};

  for (uint i{0}; i < colours_result_vecs.size(); i++)
    colours_result_vecs[i] =
        SSE2::multiply_floats(colours_seq_vecs[i], bc_component_vecs[i]);

  const auto result_vec{
      SSE2::convert_to_ints(SSE2::add_floats(colours_result_vecs))};

  const auto result_int{SSE2::pack_as_int(result_vec)};

  Colour colour{};
  memcpy(&colour, &result_int, sizeof(result_int));

  return colour;
#endif
}

Colour PixelProcessor::interpolate_texture(const BarycentricCoords &bc) {
#ifdef NO_SIMD
  const auto w_t{bc.a / rt.clip[0].w + bc.b / rt.clip[1].w +
                 bc.g / rt.clip[2].w};

  const auto uv_x{(bc.a * abc_t[0].x + bc.b * abc_t[1].x + bc.g * abc_t[2].x) /
                  w_t};

  const auto uv_y{(bc.a * abc_t[0].y + bc.b * abc_t[1].y + bc.g * abc_t[2].y) /
                  w_t};
#endif

#ifdef USING_SIMD_AVX2
  const auto bc_vec256{
      AVX2::set_floats(0.0f, bc.g, bc.b, bc.a, 0.0f, bc.g, bc.b, bc.a)};

  auto w_t_vec256{AVX2::divide_floats(bc_vec256, clip_w_seq_vec256)};

  w_t_vec256 =
      AVX2::set_float(SSE2::sum_floats(AVX2::extract_floats128<0>(w_t_vec256)));

  const auto uvs_vec256{AVX2::multiply_floats(
      abc_t_seq_vec256, AVX2::divide_floats(bc_vec256, w_t_vec256))};

  const auto uv_x{SSE2::sum_floats(AVX2::extract_floats128<0>(uvs_vec256))};
  const auto uv_y{SSE2::sum_floats(AVX2::extract_floats128<1>(uvs_vec256))};
#else
#ifdef USING_SIMD_SSE2
  auto bc_vec{SSE2::set_floats(0.0f, bc.g, bc.b, bc.a)};

  auto w_t_vec{SSE2::divide_floats(bc_vec, clip_w_seq_vec)};
  w_t_vec = SSE2::set_float(SSE2::sum_floats(w_t_vec));

  bc_vec = SSE2::divide_floats(bc_vec, w_t_vec);

  const auto uv_x_vec = SSE2::multiply_floats(abc_t_x_seq_vec, bc_vec);
  const auto uv_y_vec = SSE2::multiply_floats(abc_t_y_seq_vec, bc_vec);

  const auto uv_x{SSE2::sum_floats(uv_x_vec)};
  const auto uv_y{SSE2::sum_floats(uv_y_vec)};
#endif
#endif

  auto t_x{static_cast<int>(uv_x * texture_size.x)};
  auto t_y{static_cast<int>(uv_y * texture_size.y)};

  return rt.triangle.diffuse_texture->get_pixel({t_x, t_y});
}

void PixelProcessor::process_pixel(const glm::ivec2 &pos,
                                   const BarycentricCoords &bc, float z) {
  if (z >= render_target.z_buffer.get(pos))
    return;

  render_target.z_buffer.set(pos, z);

  Colour colour{};

  if (is_texured)
    colour = interpolate_texture(bc);
  else
    colour = interpolate_colour(bc);

  render_target.frame_buffer.set_pixel(pos, colour);
}

#ifdef NO_SIMD
void PixelProcessor::iterate_pixels(int y) {
  auto w0{w_row[0]};
  auto w1{w_row[1]};
  auto w2{w_row[2]};

  bool was_inside{false};

  for (; x < rt.box.max.x; x++) {
    const auto is_inside{
        (w0 + rt.bias[0] | w1 + rt.bias[1] | w2 + rt.bias[2]) >= 0};

    if (is_inside) {
      was_inside = true;

      const BarycentricCoords bc{w0, w1, w2, rt.area};

      const auto z{bc.a * rt.clip[0].z + bc.b * rt.clip[1].z +
                   bc.g * rt.clip[2].z};

      process_pixel({x, y}, bc, z);
    }

    else if (was_inside) {
      return;
    }

    w0 += rt.delta_w[0].x;
    w1 += rt.delta_w[1].x;
    w2 += rt.delta_w[2].x;
  }
}
#endif

#ifdef USING_SIMD_SSE2
bool PixelProcessor::pixel_is_inside_mask(uint i, int mask) {
  return ((mask >> 4 * i) & 0xF) == 0xF;
}

SSE2::Array<float>
PixelProcessor::interpolate_z_sse2(const std::array<__m128, 3> &bc_vecs) {
  return interpolate_z<SSE2>(bc_vecs, clip_z_vec);
}

std::array<SSE2::Array<int>, 4>
PixelProcessor::interpolate_colour_sse2(const std::array<__m128, 3> &bc_vecs) {
  return interpolate_colour<SSE2>(bc_vecs, colours_vecs);
}

std::pair<SSE2::Array<int>, SSE2::Array<int>>
PixelProcessor::interpolate_texture_sse2(const std::array<__m128, 3> &bc_vecs) {

  return interpolate_texture<SSE2>(texture_size_x_vec, texture_size_y_vec,
                                        bc_vecs, clip_w_vec, abc_t_x_vecs,
                                        abc_t_y_vecs);
}

void PixelProcessor::process_pixels_sse2(int y, int is_inside_mask,
                                         const std::array<__m128, 3> &bc_vecs) {

  return process_pixels<SSE2>(y, is_inside_mask, bc_vecs);
}

void PixelProcessor::iterate_pixels_sse2(int y) {
  iterate_pixels<SSE2>(y, area_vec, bias_vecs, delta_w_x_init_vecs,
                            delta_w_x_step_vecs);
}

void PixelProcessor::iterate_pixels_sequentially_sse2(int y) {
  for (; x < rt.box.max.x; x++) {
    auto is_inside_vec{SSE2::add_ints(w_seq_vec, bias_seq_vec)};

    auto is_inside_mask{SSE2::move_mask_int8(
        SSE2::compare_ints_gt(is_inside_vec, SSE2::minus_one_ints))};

    auto is_inside{(is_inside_mask == 0xFFFF)};

    if (is_inside) {
      was_inside = true;

      const auto bc_vec{SSE2::divide_floats(SSE2::convert_to_floats(w_seq_vec),
                                            area_seq_vec)};

      ALIGN_SSE2 std::array<float, 4> bc_components{};
      SSE2::store_floats(bc_components.data(), bc_vec);

      const BarycentricCoords bc{bc_components[0], bc_components[1],
                                 bc_components[2]};

      const auto z_vec{SSE2::multiply_floats(bc_vec, clip_z_seq_vec)};
      const auto z{SSE2::sum_floats(z_vec)};

      process_pixel({x, y}, bc, z);
    }

    else if (was_inside) {
      return;
    }

    w_seq_vec = SSE2::add_ints(w_seq_vec, delta_w_x_seq_vec);
  }
}
#endif

#ifdef USING_SIMD_AVX2
AVX2::Array<float>
PixelProcessor::interpolate_z_avx2(const std::array<__m256, 3> &bc_vec256s) {

  return interpolate_z<AVX2>(bc_vec256s, clip_z_vec256s);
}

std::array<AVX2::Array<int>, 4> PixelProcessor::interpolate_colour_avx2(
    const std::array<__m256, 3> &bc_vec256s) {

  return interpolate_colour<AVX2>(bc_vec256s, colours_vec256s);
}

std::pair<AVX2::Array<int>, AVX2::Array<int>>
PixelProcessor::interpolate_texture_avx2(
    const std::array<__m256, 3> &bc_vec256s) {

  return interpolate_texture<AVX2>(
      texture_size_x_vec256, texture_size_y_vec256, bc_vec256s, clip_w_vec256s,
      abc_t_x_vec256s, abc_t_y_vec256s);
}

void PixelProcessor::process_pixels_avx2(
    int y, int is_inside_mask, const std::array<__m256, 3> &bc_vec256s) {

  process_pixels<AVX2>(y, is_inside_mask, bc_vec256s);
}

void PixelProcessor::iterate_pixels_avx2(int y) {
  iterate_pixels<AVX2>(y, area_vec256, bias_vec256s,
                            delta_w_x_init_vec256s, delta_w_x_step_vec256s);
}
#endif

PixelProcessor::PixelProcessor(RenderTarget &render_target,
                               const RenderTriangle &rt)
    : render_target{render_target}, rt{rt},
      is_texured(rt.triangle.diffuse_texture) {

  if (is_texured)
    texture_size = rt.triangle.diffuse_texture->get_size();

#ifdef NO_SIMD
  w_row = rt.w_row;
#endif

#ifdef USING_SIMD_SSE2
  area_seq_vec = SSE2::set_float(static_cast<float>(rt.area));
  bias_seq_vec = SSE2::set_ints(0, rt.bias[2], rt.bias[1], rt.bias[0]);
  w_row_seq_vec = SSE2::set_ints(0, rt.w_row[2], rt.w_row[1], rt.w_row[0]);

  clip_z_seq_vec =
      SSE2::set_floats(1.0f, rt.clip[2].z, rt.clip[1].z, rt.clip[0].z);

  delta_w_x_seq_vec =
      SSE2::set_ints(0, rt.delta_w[2].x, rt.delta_w[1].x, rt.delta_w[0].x);

  delta_w_y_seq_vec =
      SSE2::set_ints(0, rt.delta_w[2].y, rt.delta_w[1].y, rt.delta_w[0].y);

  area_vec = SSE2::set_float(static_cast<float>(rt.area));

  if (is_texured) {
    clip_w_seq_vec =
        SSE2::set_floats(1.0f, rt.clip[2].w, rt.clip[1].w, rt.clip[0].w);

    const auto uvs_x_vec{SSE2::set_floats(0.0f, rt.triangle.uvs[2].x,
                                          rt.triangle.uvs[1].x,
                                          rt.triangle.uvs[0].x)};

    const auto uvs_y_vec{SSE2::set_floats(0.0f, rt.triangle.uvs[2].y,
                                          rt.triangle.uvs[1].y,
                                          rt.triangle.uvs[0].y)};

    abc_t_x_seq_vec = SSE2::divide_floats(uvs_x_vec, clip_w_seq_vec);
    abc_t_y_seq_vec = SSE2::divide_floats(uvs_y_vec, clip_w_seq_vec);

    texture_size_x_vec =
        SSE2::set_float(static_cast<float>(texture_size.x) - 0.5f);

    texture_size_y_vec =
        SSE2::set_float(static_cast<float>(texture_size.y) - 0.5f);
  }
#endif

#ifdef USING_SIMD_AVX2
  area_vec256 = AVX2::set_float(static_cast<float>(rt.area));

  if (is_texured) {
    clip_w_seq_vec256 =
        AVX2::set_floats(1.0f, rt.clip[2].w, rt.clip[1].w, rt.clip[0].w, 1.0f,
                         rt.clip[2].w, rt.clip[1].w, rt.clip[0].w);

    const auto uvs_vec256 = AVX2::set_floats(
        0.0f, rt.triangle.uvs[2].y, rt.triangle.uvs[1].y, rt.triangle.uvs[0].y,
        0.0f, rt.triangle.uvs[2].x, rt.triangle.uvs[1].x, rt.triangle.uvs[0].x);

    abc_t_seq_vec256 = AVX2::divide_floats(uvs_vec256, clip_w_seq_vec256);

    texture_size_x_vec256 =
        AVX2::set_float(static_cast<float>(texture_size.x) - 0.5f);

    texture_size_y_vec256 =
        AVX2::set_float(static_cast<float>(texture_size.y) - 0.5f);
  }
#endif

  for (uint i{0}; i < 3; i++) {
#ifdef NO_SIMD
    abc_t[i] = rt.triangle.uvs[i] / rt.clip[i].w;
#endif

#ifdef USING_SIMD_SSE2
    colours_seq_vecs[i] = SSE2::set_floats(static_cast<float>(rt.colours[i].a),
                                           static_cast<float>(rt.colours[i].b),
                                           static_cast<float>(rt.colours[i].g),
                                           static_cast<float>(rt.colours[i].r));

    colours_vecs[0][i] = SSE2::set_float(static_cast<float>(rt.colours[i].r));
    colours_vecs[1][i] = SSE2::set_float(static_cast<float>(rt.colours[i].g));
    colours_vecs[2][i] = SSE2::set_float(static_cast<float>(rt.colours[i].b));
    colours_vecs[3][i] = SSE2::set_float(static_cast<float>(rt.colours[i].a));

    bias_vecs[i] = SSE2::set_int(rt.bias[i]);

    clip_z_vec[i] = SSE2::set_float(rt.clip[i].z);
    clip_w_vec[i] = SSE2::set_float(rt.clip[i].w);

    delta_w_x_init_vecs[i] = SSE2::set_ints(
        rt.delta_w[i].x * 3, rt.delta_w[i].x * 2, rt.delta_w[i].x, 0);

    delta_w_x_step_vecs[i] = SSE2::set_int(rt.delta_w[i].x * SSE2::LANE_WIDTH);

    if (is_texured) {
      abc_t_x_vecs[i] = SSE2::set_float(rt.triangle.uvs[i].x / rt.clip[i].w);
      abc_t_y_vecs[i] = SSE2::set_float(rt.triangle.uvs[i].y / rt.clip[i].w);
    }
#endif

#ifdef USING_SIMD_AVX2
    colours_vec256s[0][i] =
        AVX2::set_float(static_cast<float>(rt.colours[i].r));

    colours_vec256s[1][i] =
        AVX2::set_float(static_cast<float>(rt.colours[i].g));

    colours_vec256s[2][i] =
        AVX2::set_float(static_cast<float>(rt.colours[i].b));

    colours_vec256s[3][i] =
        AVX2::set_float(static_cast<float>(rt.colours[i].a));

    bias_vec256s[i] = AVX2::set_int(rt.bias[i]);

    clip_z_vec256s[i] = AVX2::set_float(rt.clip[i].z);
    clip_w_vec256s[i] = AVX2::set_float(rt.clip[i].w);

    delta_w_x_init_vec256s[i] = AVX2::set_ints(
        rt.delta_w[i].x * 7, rt.delta_w[i].x * 6, rt.delta_w[i].x * 5,
        rt.delta_w[i].x * 4, rt.delta_w[i].x * 3, rt.delta_w[i].x * 2,
        rt.delta_w[i].x, 0);

    delta_w_x_step_vec256s[i] =
        AVX2::set_int(rt.delta_w[i].x * AVX2::LANE_WIDTH);

    if (is_texured) {
      abc_t_x_vec256s[i] = AVX2::set_float(rt.triangle.uvs[i].x / rt.clip[i].w);
      abc_t_y_vec256s[i] = AVX2::set_float(rt.triangle.uvs[i].y / rt.clip[i].w);
    }
#endif
  }
}

void PixelProcessor::iterate_x(int y) {
  x = rt.box.min.x;

#ifdef USING_SIMD_SSE2
  was_inside = false;
  is_outside_right = false;

  w_seq_vec = w_row_seq_vec;
#endif

#ifdef NO_SIMD
  iterate_pixels(y);
#endif

#ifdef USING_SIMD_AVX2
  iterate_pixels_avx2(y);
#endif

#ifdef USING_SIMD_SSE2
  iterate_pixels_sse2(y);
  iterate_pixels_sequentially_sse2(y);
#endif
}

void PixelProcessor::step_y() {
#ifdef NO_SIMD_SSE2
  w_row[0] += rt.delta_w[0].y;
  w_row[1] += rt.delta_w[1].y;
  w_row[2] += rt.delta_w[2].y;
#endif

#ifdef USING_SIMD_SSE2
  w_row_seq_vec = SSE2::add_ints(w_row_seq_vec, delta_w_y_seq_vec);
#endif
}

} // namespace Archa
