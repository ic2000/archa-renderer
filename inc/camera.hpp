#pragma once

#include "config.hpp"

#include <glm/glm.hpp>

#include "transform.hpp"

namespace Archa {

class Camera : public Transform {
  glm::mat4 pitch{1};
  glm::mat4 yaw{1};
  glm::mat4 roll{1};

  float fov{70.0f};
  float z_near{0.1f};
  float z_far{1000.0f};

public:
  float get_fov() const;
  float get_z_near() const;
  float get_z_far() const;
};

} // namespace Archa
