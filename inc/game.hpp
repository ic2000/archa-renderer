#pragma once

#include "config.hpp"

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <imgui.h>

#include "scene.hpp"
#include "types.hpp"
#include "viewport.hpp"

namespace Archa {

class Game {
  sf::RenderWindow window{};

  float ui_scale{1.0f};
  float resolution_scale{1.0f};

  ImFont *font{};

  sf::View game_view{};
  sf::View hud_view{};

  Camera camera{};
  Scene scene{};

  Viewport viewport{};

  void scale_ui(float scale);

  void setup_scene();

  void resize_game_view(const glm::ivec2 &size);

  void show_fps(float fps);
  void render_viewport();

public:
  void init(const glm::ivec2 &size, const std::string &title,
            float resolution_scale = 1.0f);

  void run(uint max_frame_rate = 0);
};

} // namespace Archa
