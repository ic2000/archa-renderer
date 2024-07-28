#pragma once

#include "config.hpp"

#include <array>
#include <glm/glm.hpp>
#include <vector>

#include "colour.hpp"
#include "constants.hpp"
#include "intrinsics.hpp"
#include "types.hpp"

namespace Archa {

class FrameBuffer {
  glm::ivec2 size{};
  ALIGN_SIMD std::vector<uint8> pixels{};

public:
  void create(const glm::ivec2 &size);

  void set_pixel(const glm::ivec2 &pos, const Colour &colour);

#ifdef USING_SIMD_SSE2
  void set_pixels(const glm::ivec2 &pos, const SSE2::Array<Colour> &colours);
#endif

#ifdef USING_SIMD_AVX2
  void set_pixels(const glm::ivec2 &pos, const AVX2::Array<Colour> &colours);
#endif

  const uint8 *get_pixels() const;
};

} // namespace Archa
