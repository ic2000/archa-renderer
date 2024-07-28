#pragma once

#include "config.hpp"

#include <vector>

#include "model_instance.hpp"

namespace Archa {

struct Scene {
  std::vector<ModelInstance> model_instances{};
};

} // namespace Archa
