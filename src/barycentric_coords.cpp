#include "barycentric_coords.hpp"

BarycentricCoords::BarycentricCoords(float a, float b, float g)
    : a{a}, b{b}, g{g} {}

BarycentricCoords::BarycentricCoords(int w0, int w1, int w2, int area)
    : a{w0 / static_cast<float>(area)}, b{w1 / static_cast<float>(area)},
      g{w2 / static_cast<float>(area)} {}
