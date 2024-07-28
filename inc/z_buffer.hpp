#pragma once

#include "config.hpp"

#include <vector>

namespace Archa {

class ZBuffer {
  glm::ivec2 size{};
  ALIGN_SIMD std::vector<float> data{};

public:
  void create(const glm::ivec2 &size);

  void clear_single(int i);

#ifdef USING_SIMD_AVX2
  void clear_lane_avx2(int i);
#endif

#ifdef USING_SIMD_SSE2
  void clear_lane_sse2(int i);
#endif

  // void clear();

  void set(const glm::ivec2 &pos, float value);
  float get(const glm::ivec2 &pos) const;
};

} // namespace Archa
