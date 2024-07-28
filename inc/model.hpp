#pragma once

#include "config.hpp"

#include <SFML/Graphics/Image.hpp>
#include <filesystem>
#include <string>
#include <vector>

#include "resource.hpp"
#include "triangle.hpp"
#include "vertex.hpp"

namespace Archa {

struct Model : public Resource {
  std::string name{};
  std::vector<Vertex> vertices{};
  std::vector<Triangle> triangles{};

  void load(const std::filesystem::path &file_path) override;
};

} // namespace Archa
