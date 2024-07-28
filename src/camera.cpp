#include "camera.hpp"

namespace Archa {

float Camera::get_fov() const { return fov; }
float Camera::get_z_near() const { return z_near; }
float Camera::get_z_far() const { return z_far; }

} // namespace Archa
