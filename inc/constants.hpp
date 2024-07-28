#pragma once

#include "config.hpp"

#include <filesystem>

namespace Archa {

constexpr auto RGBA_CHANNEL_COUNT{4};

namespace DIR {

const auto RESOURCES{std::filesystem::path("resources")};
const auto FONTS = RESOURCES / "fonts";
const auto TEXTURES = RESOURCES / "textures";
const auto MODELS = RESOURCES / "models";

} // namespace DIR

namespace FONT {

constexpr auto DEFAULT_NAME{"Pixel12x10-v1.1.0.ttf"};
const auto DEFAULT_PATH{DIR::FONTS / DEFAULT_NAME};

constexpr auto DEFAULT_SIZE{16.0f};

} // namespace FONT

} // namespace Archa
