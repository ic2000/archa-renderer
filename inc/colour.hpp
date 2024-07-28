#pragma once

#include "config.hpp"

#include "types.hpp"

namespace Archa {

#pragma pack(push, 1)
struct Colour {
  uint8 r{};
  uint8 g{};
  uint8 b{};
  uint8 a{255};

  static const Colour Black;
  static const Colour White;
};
#pragma pack(pop)

} // namespace Archa
