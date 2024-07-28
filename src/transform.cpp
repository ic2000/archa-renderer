#include "transform.hpp"

namespace Archa {

void Transform::set_position(const glm::vec3 &position) {
  this->position = glm::mat4{1};
  translate(position);
}

void Transform::set_rotation(const glm::vec3 &rotation) {
  this->rotation = glm::mat4{1};
  rotate(rotation);
}

void Transform::set_scale(const glm::vec3 &scale) {
  this->scale = glm::mat4{1};
  scale_by(scale);
}

void Transform::translate(const glm::vec3 &translation) {
  position = glm::translate(position, translation);
  is_dirty = true;
}

void Transform::rotate(const glm::vec3 &rotation) {
  this->rotation = glm::rotate(this->rotation, rotation.x, glm::vec3{1, 0, 0});
  this->rotation = glm::rotate(this->rotation, -rotation.y, glm::vec3{0, 1, 0});
  this->rotation = glm::rotate(this->rotation, rotation.z, glm::vec3{0, 0, 1});
  is_dirty = true;
}

void Transform::scale_by(const glm::vec3 &scale) {
  this->scale = glm::scale(this->scale, scale);
  is_dirty = true;
}

const glm::mat4 &Transform::get_position() const { return position; }
const glm::mat4 &Transform::get_rotation() const { return rotation; }
const glm::mat4 &Transform::get_scale() const { return scale; }

const glm::mat4 &Transform::get_transform() const {
  if (is_dirty) {
    transform = position * rotation * scale;
    is_dirty = false;
  }

  return transform;
}

} // namespace Archa
