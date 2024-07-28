#include "frame_buffer.hpp"

#include "constants.hpp"

namespace Archa {

void FrameBuffer::create(const glm::ivec2 &size) {
  this->size = size;

  pixels.resize(static_cast<uint>(this->size.x * this->size.y) *
                RGBA_CHANNEL_COUNT);
}

void FrameBuffer::set_pixel(const glm::ivec2 &pos, const Colour &colour) {
  const auto index{static_cast<uint>(pos.y * size.x + pos.x) *
                   RGBA_CHANNEL_COUNT};

  memcpy(&pixels[index], &colour, sizeof(Colour));
}

#ifdef USING_SIMD_SSE2
void FrameBuffer::set_pixels(const glm::ivec2 &pos,
                             const SSE2::Array<Colour> &colours) {

  const auto index{static_cast<uint>(pos.y * size.x + pos.x) *
                   RGBA_CHANNEL_COUNT};

  const auto colours_vec{
      SSE2::load_ints(reinterpret_cast<const int *>(colours.data()))};

  SSE2::store_ints(reinterpret_cast<int *>(&pixels[index]), colours_vec);
}
#endif

#ifdef USING_SIMD_AVX2
void FrameBuffer::set_pixels(const glm::ivec2 &pos,
                             const AVX2::Array<Colour> &colours) {

  const auto index{static_cast<uint>(pos.y * size.x + pos.x) *
                   RGBA_CHANNEL_COUNT};

  const auto colours_vec{
      AVX2::load_ints(reinterpret_cast<const int *>(colours.data()))};

  AVX2::store_ints(reinterpret_cast<int *>(&pixels[index]), colours_vec);
}
#endif

const uint8 *FrameBuffer::get_pixels() const { return pixels.data(); }

} // namespace Archa
