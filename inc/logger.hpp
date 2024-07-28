#pragma once

#include "config.hpp"

#include <iostream>
#include <source_location>
#include <syncstream>

namespace Archa {

class Logger {
  std::osyncstream out;

  void log_metadata(const std::source_location &location);

  void log_level(const std::source_location &location,
                 const std::string &level);

public:
  Logger(std::ostream &stream = std::cout);

  Logger &
  info(const std::source_location &location = std::source_location::current());

  Logger &
  warn(const std::source_location &location = std::source_location::current());

  Logger &
  error(const std::source_location &location = std::source_location::current());

  Logger &
  fatal(const std::source_location &location = std::source_location::current());

  template <typename T> Logger &operator<<(const T &value) {
    out << value;
    return *this;
  }

  // override << for manipulators like std::endl
  Logger &operator<<(std::ostream &(*manip)(std::ostream &)) {
    manip(out);
    return *this;
  }
};

} // namespace Archa
