#include "bin.hpp"

#include "colour.hpp"

namespace Archa {

void Bin::create(const glm::ivec2 &pos, const glm::ivec2 &size,
                 const Colour &fill_colour) {

  this->pos = pos;
  this->size = size;

  this->fill_colour = fill_colour;
}

const glm::ivec2 &Bin::get_pos() const { return pos; }
const glm::ivec2 &Bin::get_size() const { return size; }

const Colour &Bin::get_fill_colour() const { return fill_colour; }

} // namespace Archa
