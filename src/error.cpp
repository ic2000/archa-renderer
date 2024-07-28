#include "error.hpp"

#include "logger.hpp"

namespace Archa {

void fatal_error(const std::string &message) {
  Logger().fatal() << message << std::endl;
  std::exit(EXIT_FAILURE);
}

} // namespace Archa
