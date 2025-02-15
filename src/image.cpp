#include "image.hpp"

#include "constants.hpp"
#include "error.hpp"
#include "logger.hpp"

namespace Archa {

void Image::load(const std::filesystem::path &file_path) {
  if (!image.loadFromFile(file_path.string()))
    fatal_error("Failed to load image: " + file_path.string());

  size = {image.getSize().x, image.getSize().y};
  pixels = image.getPixelsPtr();

  Logger().info() << "Loaded image: " << file_path
                  << ", size: " << image.getSize().x << "x" << image.getSize().y
                  << '\n';
}

const glm::ivec2 &Image::get_size() const { return size; }
const uint8 *Image::get_pixels() const { return pixels; }

const Colour &Image::get_pixel(const glm::ivec2 &pos) const {
  const auto index{static_cast<uint>(pos.y * size.x + pos.x) *
                   RGBA_CHANNEL_COUNT};

  return *reinterpret_cast<const Colour *>(&pixels[index]);
}

} // namespace Archa
