#include "model.hpp"
#include "glm/fwd.hpp"

#include <cmath>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "colour.hpp"
#include "error.hpp"
#include "image.hpp"
#include "logger.hpp"
#include "resource_manager.hpp"
#include "types.hpp"

namespace Archa {

static bool is_missing_normal(const glm::vec3 &normal) {
  return glm::length(normal) == 0.0f;
}

static bool has_missing_normals(const std::array<glm::vec3, 3> &normals) {
  return is_missing_normal(normals[0]) || is_missing_normal(normals[1]) ||
         is_missing_normal(normals[2]);
}

static void add_verticies(const tinyobj::attrib_t &attrib,
                          std::vector<Vertex> &vertices) {
  for (uint i{0}; i < attrib.vertices.size(); i += 3) {
    const auto vx{attrib.vertices[i + 0]};
    const auto vy{attrib.vertices[i + 1]};
    const auto vz{attrib.vertices[i + 2]};

    const auto red{attrib.colors[i + 0]};
    const auto green{attrib.colors[i + 1]};
    const auto blue{attrib.colors[i + 2]};

    Colour colour{static_cast<uint8>(red * 255),
                  static_cast<uint8>(green * 255),
                  static_cast<uint8>(blue * 255)};

    vertices.emplace_back(Vertex({vx, vy, -vz}, colour));
  }
}

static void set_normals(const tinyobj::index_t &idx, Triangle &triangle, uint v,
                        const tinyobj::attrib_t &attrib) {
  if (idx.normal_index >= 0) {
    triangle.normals[v].x =
        attrib.normals[3 * static_cast<uint>(idx.normal_index) + 0];

    triangle.normals[v].y =
        attrib.normals[3 * static_cast<uint>(idx.normal_index) + 1];

    triangle.normals[v].z =
        -attrib.normals[3 * static_cast<uint>(idx.normal_index) + 2];

    triangle.normals[v] = glm::normalize(triangle.normals[v]);
  }
}

static void set_tex_coords(const tinyobj::index_t &idx, Triangle &triangle,
                           uint v, const tinyobj::attrib_t &attrib) {
  if (idx.texcoord_index >= 0) {
    triangle.uvs[v].x =
        attrib.texcoords[2 * static_cast<uint>(idx.texcoord_index) + 0];

    triangle.uvs[v].y =
        attrib.texcoords[2 * static_cast<uint>(idx.texcoord_index) + 1];
  }
}

static glm::vec2 wrap_uv(const glm::vec2 &uv) {
  glm::vec2 result{std::fmod(uv.x, 1.0f), std::fmod(uv.y, 1.0f)};

  if (result.x < 0.0f)
    result.x += 1.0f;

  if (result.y < 0.0f)
    result.y += 1.0f;

  return result;
}

static glm::vec3 calculate_triangle_normal(const glm::vec3 &v1,
                                           const glm::vec3 &v2,
                                           const glm::vec3 &v3) {
  const auto e1{v2 - v1};
  const auto e2{v3 - v1};
  const auto normal{glm::normalize(glm::cross(e1, e2))};

  return normal;
}

void Model::load(const std::filesystem::path &file_path) {
  tinyobj::ObjReaderConfig reader_config{};
  // reader_config.mtl_search_path = path.parent_path().string();
  tinyobj::ObjReader reader{};

  if (!reader.ParseFromFile(file_path.string(), reader_config))
    if (!reader.Error().empty())
      fatal_error("TinyObjReader: " + reader.Error());

  if (!reader.Warning().empty())
    Logger().warn() << "TinyObjReader: " << reader.Warning() << '\n';

  Logger().info() << "Loading model: " << file_path << '\n';

  name = file_path.string();

  const auto &attrib{reader.GetAttrib()};
  const auto &shapes{reader.GetShapes()};
  const auto &materials{reader.GetMaterials()};

  add_verticies(attrib, vertices);

  for (uint s{0}; s < shapes.size(); s++) {
    uint index_offset{0};

    for (uint f{0}; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      const auto fv{shapes[s].mesh.num_face_vertices[f]};

      if (fv != 3)
        fatal_error("Only triangles are supported");

      Triangle triangle{};

      for (uint v{0}; v < fv; v++) {
        const auto idx = shapes[s].mesh.indices[index_offset + v];
        triangle.i[v] = static_cast<uint>(idx.vertex_index);

        set_normals(idx, triangle, v, attrib);
        set_tex_coords(idx, triangle, v, attrib);
      }

      const auto material_id{shapes[s].mesh.material_ids[f]};
      if (material_id >= 0) {
        const auto &material{materials[static_cast<uint>(material_id)]};
        const auto &diffuse{material.diffuse};

        for (uint v{0}; v < fv; v++) {
          vertices[triangle.i[v]].colour = {
              static_cast<uint8>(diffuse[0] * 255),
              static_cast<uint8>(diffuse[1] * 255),
              static_cast<uint8>(diffuse[2] * 255)};
        }

        if (!material.diffuse_texname.empty()) {
          const auto diffuse_texture_path{file_path.parent_path() /
                                          material.diffuse_texname};

          triangle.diffuse_texture = ResourceManager::load<Image>(
              diffuse_texture_path.string(), diffuse_texture_path);

          for (uint i{0}; i < triangle.uvs.size(); i++) {
            triangle.uvs[i] = wrap_uv(triangle.uvs[i]);

            triangle.uvs[i].y = 1.0f - triangle.uvs[i].y;
          }
        }

        std::swap(triangle.i[1], triangle.i[2]);
        std::swap(triangle.normals[1], triangle.normals[2]);
        std::swap(triangle.uvs[1], triangle.uvs[2]);

        if (has_missing_normals(triangle.normals)) {
          const auto new_normal{calculate_triangle_normal(
              vertices[triangle.i[0]], vertices[triangle.i[1]],
              vertices[triangle.i[2]])};

          for (auto &normal : triangle.normals)
            normal = new_normal;
        }

        triangles.push_back(std::move(triangle));

        index_offset += fv;
      }
    }
  }

  Logger().info() << "Loaded model: " << file_path << ", " << vertices.size()
                  << " vertices, " << triangles.size() << " triangles"
                  << '\n';
}

} // namespace Archa
