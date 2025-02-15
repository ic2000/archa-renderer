#include "game.hpp"
#include "config.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <imgui-SFML.h>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include "constants.hpp"
#include "error.hpp"
#include "font.hpp"
#include "image.hpp"
#include "logger.hpp"
#include "model_instance.hpp"
#include "resource_manager.hpp"
#include "triangle.hpp"

namespace Archa {

static float get_dpi_scale_factor() {
#ifdef _WIN32
  auto screen = GetDC(0);
  const auto dpiX = GetDeviceCaps(screen, LOGPIXELSX);
  ReleaseDC(0, screen);

  constexpr auto default_dpi{96.0f};
  return static_cast<float>(dpiX) / default_dpi;
#else
  return 1.0f;
#endif // _WIN32
}

void Game::scale_ui(float scale) {
  font = ResourceManager::load<Font>("default", FONT::DEFAULT_PATH)
             ->get_font(FONT::DEFAULT_SIZE * scale);

  (void)ImGui::SFML::UpdateFontTexture(); // needed to circumvent ImGUI SFML bug

  ImGui::GetStyle().ScaleAllSizes(scale / ui_scale);
  ui_scale = scale;
}

void Game::setup_scene() {
  const auto player_model = ResourceManager::load<Model>(
      DIR::MODELS / "femalesoldier" / "femalesoldier.obj");

  // for (uint i{0}; i < 20; i++) {
  ModelInstance player_instance{*player_model};

  // player_instance.set_scale({0.05f, 0.05f, 0.05f});
  player_instance.set_scale({5.0f, 5.0f, 5.0f});

  player_instance.translate({0.0f, -1.5f, 4.0f});

  // player_instance.translate({-9.5f, -1.5f, 8.0f});
  // player_instance.translate({i, 0.0f, 0.0f});

  scene.model_instances.push_back(player_instance);
  // }

  static Model model{};

  model.vertices = {
      {{-0.5f, -0.5f, 0.0f}, {255, 0, 0}},
      {{-0.5f, 0.5f, 0.0f}, {0, 255, 0}},
      {{0.5f, 0.5f, 0.0f}, {0, 0, 255}},
      {{0.5f, -0.5f, 0.0f}, {255, 255, 0}},
  };

  Triangle triangle1{{0, 1, 2}};

  triangle1.uvs[0] = {0, 1};
  triangle1.uvs[1] = {0, 0};
  triangle1.uvs[2] = {1, 0};

  triangle1.diffuse_texture =
      ResourceManager::load<Image>(DIR::TEXTURES / "floor.png");

  model.triangles.push_back(triangle1);

  Triangle triangle2{{0, 2, 3}};
  triangle2.uvs[0] = {0, 1};
  triangle2.uvs[1] = {1, 0};
  triangle2.uvs[2] = {1, 1};

  // triangle2.diffuse_texture =
  //     ResourceManager::load<Image>("floor.png", DIR::TEXTURES / "floor.png");

  model.triangles.push_back(triangle2);

  // for (int i = 0; i < 25; i++) {
  ModelInstance model_instance{model};

  // model_instance.translate({-2.5f, 0.0f, 5.0f});

  // model_instance.translate(
  // {-1.0f + (static_cast<float>(i) * 0.1f), 0.0f, 0.0f});

  scene.model_instances.push_back(model_instance);
  // }
}

void Game::resize_game_view(const glm::ivec2 &size) {
  const glm::ivec2 resolution{glm::vec2(size) * resolution_scale};

  game_view.setSize(static_cast<float>(resolution.x),
                    static_cast<float>(resolution.y));

  game_view.setCenter(game_view.getSize().x / 2.0f,
                      game_view.getSize().y / 2.0f);

  viewport.create(resolution, std::thread::hardware_concurrency());
}

void Game::show_fps(float fps) {
  ImGui::SetNextWindowPos(ImVec2(10, 10));

  ImGui::Begin("FPS", nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_AlwaysAutoResize);

  ImGui::Text("FPS: %d", static_cast<int>(fps));
  ImGui::End();
}

void Game::render_viewport() {
  viewport.render();

  const auto &texture{viewport.get_texture()};
  window.draw(sf::Sprite{texture});
}

void Game::init(const glm::ivec2 &size, const std::string &title,
                float resolution_scale) {
  this->resolution_scale = resolution_scale;

  Logger().info() << "Creating window of size " << size.x << "x" << size.y
                  << '\n';

  window.create({static_cast<uint>(size.x), static_cast<uint>(size.y)},
                title.c_str());

  if (!ImGui::SFML::Init(window))
    fatal_error("Failed to initialize ImGui-SFML");

  scale_ui(get_dpi_scale_factor());

  setup_scene();

  viewport.set_scene(scene);
  viewport.set_camera(camera);

  resize_game_view(size);
}

void Game::run(uint max_frame_rate) {
  if (max_frame_rate > 0)
    window.setFramerateLimit(max_frame_rate);

  sf::Clock delta_clock{};

  float fps{0.0f};
  float frame_count{0.0f};
  float elapsed_time{0.0f};

  while (window.isOpen()) {
    const auto delta_time{delta_clock.restart()};
    const auto delta_seconds = delta_time.asSeconds();

    frame_count += 1.0f;
    elapsed_time += delta_seconds;

    if (elapsed_time >= 1.0f) {
      fps = frame_count;
      frame_count = 0.0f;
      elapsed_time = 0.0f;
    }

    sf::Event event{};

    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(window, event);

      if (event.type == sf::Event::Closed)
        window.close();

      else if (event.type == sf::Event::Resized) {
        auto new_size{window.getSize()};

        new_size.x = new_size.x - (new_size.x % SIMD_ALIGN_WIDTH);
        new_size.y = new_size.y - (new_size.y % SIMD_ALIGN_WIDTH);

        if (new_size.x == 0)
          new_size.x = SIMD_ALIGN_WIDTH;

        if (new_size.y == 0)
          new_size.y = SIMD_ALIGN_WIDTH;

        Logger().info() << "Resizing window to " << new_size.x << "x"
                        << new_size.y << '\n';

        window.setSize(new_size);
        resize_game_view({new_size.x, new_size.y});
      }
    }

    for (auto &model_instance : scene.model_instances)
      model_instance.rotate({0.0f, 0.5f * delta_seconds, 0.0f});

    ImGui::SFML::Update(window, delta_time);
    ImGui::PushFont(font);
    show_fps(fps);
    ImGui::PopFont();

    window.clear();

    window.setView(game_view);
    render_viewport();

    window.setView(hud_view);

    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
}

} // namespace Archa
