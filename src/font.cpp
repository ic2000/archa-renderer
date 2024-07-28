#include "font.hpp"

#include "error.hpp"
#include "logger.hpp"
#include "util.hpp"

namespace Archa {

void Font::load(const std::filesystem::path &file_path) {
  font_data = read_binary_file(file_path);
  Logger().info() << "Loaded font: " << file_path << '\n';
}

ImFont *Font::get_font(float size) const {
  ImFontConfig font_cfg{};
  font_cfg.FontDataOwnedByAtlas = false;

  auto font{ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
      reinterpret_cast<void *>(font_data.data()),
      static_cast<int>(font_data.size()), size, &font_cfg)};

  if (!font)
    fatal_error("Failed to load font from memory");

  return font;
}

} // namespace Archa
