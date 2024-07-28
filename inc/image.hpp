#pragma once

#include "config.hpp"

#include <SFML/Graphics/Image.hpp>
#include <glm/common.hpp>

#include "colour.hpp"
#include "resource.hpp"
#include "types.hpp"

namespace Archa {

class Image : public Resource {
  glm::ivec2 size{};
  const uint8 *pixels{nullptr};

public:
  sf::Image image{};

  void load(const std::filesystem::path &file_path) override;

  const glm::ivec2 &get_size() const;
  const uint8 *get_pixels() const;

  const Colour &get_pixel(const glm::ivec2 &pos) const;
};

} // namespace Archa
