#pragma once

#include "config.hpp"

#include <SFML/Graphics/Texture.hpp>

#include "resource.hpp"

namespace Archa {

class Texture : public Resource {
public:
  sf::Texture texture{};

  void load(const std::filesystem::path &file_path) override;
};

} // namespace Archa
