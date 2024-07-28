#include "texture.hpp"

#include "error.hpp"
#include "logger.hpp"

namespace Archa {

void Texture::load(const std::filesystem::path &file_path) {
  if (!texture.loadFromFile(file_path.string()))
    fatal_error("Failed to load texture: " + file_path.string());

  Logger().info() << "Loaded texture: " << file_path
                  << ", size: " << texture.getSize().x << "x"
                  << texture.getSize().y << std::endl;
}

} // namespace Archa
