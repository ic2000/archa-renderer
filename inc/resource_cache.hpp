#pragma once

#include "config.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace Archa {

template <typename T> class ResourceCache {
  std::unordered_map<std::string, std::shared_ptr<T>> cache{};

public:
  std::shared_ptr<T> get(const std::string &name) {
    auto it{cache.find(name)};

    if (it != cache.end())
      return it->second;

    return nullptr;
  }

  void add(const std::string &name, const std::shared_ptr<T> &resource) {
    cache.insert_or_assign(name, resource);
  }

  void remove(const std::string &name) { cache.erase(name); }
};

} // namespace Archa
