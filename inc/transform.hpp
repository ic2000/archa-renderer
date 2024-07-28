#pragma once

#include "config.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Archa {

class Transform {
protected:
  glm::mat4 position{1};
  glm::mat4 rotation{1};
  glm::mat4 scale{1};

  mutable glm::mat4 transform{1};
  mutable bool is_dirty{false};

public:
  void set_position(const glm::vec3 &position);
  void set_rotation(const glm::vec3 &rotation);
  void set_scale(const glm::vec3 &scale);

  void translate(const glm::vec3 &translation);
  void rotate(const glm::vec3 &rotation);
  void scale_by(const glm::vec3 &scale);

  const glm::mat4 &get_position() const;
  const glm::mat4 &get_rotation() const;
  const glm::mat4 &get_scale() const;
  const glm::mat4 &get_transform() const;
};

} // namespace Archa
