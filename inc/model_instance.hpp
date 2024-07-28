#pragma once

#include "config.hpp"

#include "model.hpp"
#include "transform.hpp"

namespace Archa {

struct ModelInstance : public Transform {
  const Model &model; // TODO: make this a reference

  ModelInstance(const Model &model);
};

} // namespace Archa
