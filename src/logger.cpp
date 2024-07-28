#include "logger.hpp"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

namespace Archa {

void Logger::log_metadata(const std::source_location &location) {
  const auto now = std::chrono::system_clock::now();
  const auto now_c = std::chrono::system_clock::to_time_t(now);

  std::tm time_info{};
  localtime_s(&time_info, &now_c);

  const auto milliseconds{std::chrono::duration_cast<std::chrono::milliseconds>(
                              now.time_since_epoch()) %
                          1000};

  char time_str[20]{};
  std::strftime(time_str, sizeof(time_str), "%T", &time_info);

  std::ostringstream oss{};

  oss << std::put_time(&time_info, "%T") << '.' << std::setw(3)
      << std::setfill('0') << milliseconds.count();

  const std::filesystem::path &file_path(location.file_name());
  const std::string &file_name{file_path.filename().string()};

  out << std::format("[{}] [{}:{}] ", oss.str(), file_name, location.line());
}

void Logger::log_level(const std::source_location &location,
                       const std::string &level) {
  log_metadata(location);
  out << "[" << level << "] ";
}

Logger::Logger(std::ostream &stream) : out(stream) {}

Logger &Logger::info(const std::source_location &location) {
  log_level(location, "INFO");
  return *this;
}

Logger &Logger::warn(const std::source_location &location) {
  log_level(location, "WARN");
  return *this;
}

Logger &Logger::error(const std::source_location &location) {
  log_level(location, "ERROR");
  return *this;
}

Logger &Logger::fatal(const std::source_location &location) {
  log_level(location, "FATAL");
  return *this;
}

}; // namespace Archa
