#pragma once

#include "config.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <type_traits>

#include "resource.hpp"
#include "resource_cache.hpp"

namespace Archa {

class ResourceManager {
  template <typename T> static ResourceCache<T> &get_cache() {
    static ResourceCache<T> cache{};
    return cache;
  }

  template <typename T> static void check_resource_type() {
    static_assert(std::is_base_of<Resource, T>::value,
                  "T must be derived from Resource");
  }

public:
  template <typename T>
  static std::shared_ptr<const T> load(const std::string &name,
                                       std::filesystem::path file_path) {
    check_resource_type<T>();

    auto resource{get_cache<T>().get(name)};

    if (!resource) {
      resource = std::make_shared<T>();
      resource->load(file_path);
      get_cache<T>().add(name, resource);
    }

    return resource;
  }

  template <typename T>
  static std::shared_ptr<const T> load(std::filesystem::path file_path) {
    check_resource_type<T>();

    return load<T>(file_path.string(), file_path);
  }

  template <typename T>
  static std::shared_ptr<const T> get(const std::string &name) {
    check_resource_type<T>();

    return get_cache<T>().get(name);
  }

  template <typename T> static void unload(const std::string &name) {
    check_resource_type<T>();

    get_cache<T>().remove(name);
  }
};

} // namespace Archa
