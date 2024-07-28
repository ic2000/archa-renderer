#pragma once

#include "config.hpp"

struct BarycentricCoords {
  float a{}, b{}, g{};

  BarycentricCoords(float a, float b, float g);
  BarycentricCoords(int w0, int w1, int w2, int area);
};
