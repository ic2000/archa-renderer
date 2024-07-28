#pragma once

#include "config.hpp"

#include <filesystem>
#include <vector>

#include "types.hpp"

namespace Archa {

template <typename T> std::vector<T> create_reserved_vector(uint capacity) {
  std::vector<T> result{};
  result.reserve(capacity);

  return result;
}

std::vector<uint8> read_binary_file(const std::filesystem::path &file_path);

} // namespace Archa
