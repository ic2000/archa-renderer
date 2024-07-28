#include "util.hpp"

#include <fstream>

#include "error.hpp"

namespace Archa {

std::vector<uint8> read_binary_file(const std::filesystem::path &file_path) {
  std::ifstream file{file_path, std::ios::binary | std::ios::ate};

  if (!file.is_open())
    fatal_error("Failed to open file: " + file_path.string());

  const auto size{file.tellg()};
  file.seekg(0, std::ios::beg);

  std::vector<uint8> buffer(static_cast<uint>(size));

  if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
    fatal_error("Failed to read file: " + file_path.string());

  return buffer;
}

} // namespace Archa
