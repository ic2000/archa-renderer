#include "z_buffer.hpp"

#include "intrinsics.hpp"
#include "types.hpp"

namespace Archa {

void ZBuffer::create(const glm::ivec2 &size) {
  this->size = size;

  data.resize(static_cast<uint>(size.x * size.y));
  // clear();
}

void ZBuffer::clear_single(int i) {
  data[static_cast<uint>(i)] = std::numeric_limits<float>::max();
}

#ifdef USING_SIMD_AVX2
void ZBuffer::clear_lane_avx2(int i) {
  static const auto z_values_vec256{
      AVX2::set_float(std::numeric_limits<float>::max())};

  AVX2::store_floats(&data[static_cast<uint>(i)], z_values_vec256);
}
#endif

#ifdef USING_SIMD_SSE2
void ZBuffer::clear_lane_sse2(int i) {
  static const auto z_values_vec{
      SSE2::set_float(std::numeric_limits<float>::max())};

  SSE2::store_floats(&data[static_cast<uint>(i)], z_values_vec);
}
#endif

// void ZBuffer::clear() {
//   auto i{0};

// #ifdef USING_SIMD_AVX2
//   for (; i < std::ssize(data) - (AVX2::LANE_WIDTH - 1); i += AVX2::LANE_WIDTH)
//     clear_lane_avx2(i);
// #endif

// #ifdef USING_SIMD_SSE2
//   for (; i < std::ssize(data) - (SSE2::LANE_WIDTH - 1); i += SSE2::LANE_WIDTH)
//     clear_lane_sse2(i);
// #endif

//   std::fill(std::begin(data) + i, std::end(data),
//             std::numeric_limits<float>::max());
// }

void ZBuffer::set(const glm::ivec2 &pos, float value) {
  data[static_cast<uint>(pos.y * size.x + pos.x)] = value;
}

float ZBuffer::get(const glm::ivec2 &pos) const {
  return data[static_cast<uint>(pos.y * size.x + pos.x)];
}

} // namespace Archa
