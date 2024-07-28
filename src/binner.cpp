#include "binner.hpp"

namespace Archa {

static const std::array<Colour, 32> BIN_COLOURS = {
    Colour(8, 8, 8),       Colour(16, 16, 16),    Colour(24, 24, 24),
    Colour(32, 32, 32),    Colour(40, 40, 40),    Colour(48, 48, 48),
    Colour(56, 56, 56),    Colour(64, 64, 64),    Colour(72, 72, 72),
    Colour(80, 80, 80),    Colour(88, 88, 88),    Colour(96, 96, 96),
    Colour(104, 104, 104), Colour(112, 112, 112), Colour(120, 120, 120),
    Colour(128, 128, 128), Colour(136, 136, 136), Colour(144, 144, 144),
    Colour(152, 152, 152), Colour(160, 160, 160), Colour(168, 168, 168),
    Colour(176, 176, 176), Colour(184, 184, 184), Colour(192, 192, 192),
    Colour(200, 200, 200), Colour(208, 208, 208), Colour(216, 216, 216),
    Colour(224, 224, 224), Colour(232, 232, 232), Colour(240, 240, 240),
    Colour(248, 248, 248), Colour(255, 255, 255)};

void Binner::split_bins(const glm::ivec2 &size, int count) {
  bins.clear();

  const auto total_bin_count{count};
  const auto total_area{size.x * size.y};

  const auto bin_area{static_cast<float>(total_area) / total_bin_count};
  const auto bin_length{static_cast<int>(std::sqrt(bin_area))};

  const auto column_count{size.x / bin_length};
  const auto column_bin_count{total_bin_count / column_count};

  auto remaining_bins{total_bin_count % column_count};

  const auto total_columns_height{bin_length * total_bin_count};

  auto bin_x{0};

  for (int i{0}; i < column_count; i++) {
    auto bin_count{0};

    if (remaining_bins) {
      bin_count = column_bin_count + 1;
      remaining_bins -= 1;
    } else {
      bin_count = column_bin_count;
    }

    const auto column_height{bin_length * bin_count};

    auto column_width{static_cast<int>(static_cast<float>(column_height) /
                                       total_columns_height * size.x)};

    column_width -= column_width % 8;

    auto bin_width{0};

    if (i == column_count - 1)
      bin_width = size.x - bin_x;
    else
      bin_width = static_cast<int>(column_width);

    auto bin_height{size.y / bin_count};

    for (int j{0}; j < bin_count; j++) {
      auto bin_y{j * bin_height};

      if (j == bin_count - 1)
        bin_height += size.y - (bin_y + bin_height);

      bins.push_back(Bin{});

      const auto bin_colour{
          BIN_COLOURS[(bins.size() - 1) % BIN_COLOURS.size()]};

      bins.back().create({bin_x, bin_y}, {bin_width, bin_height}, bin_colour);
    }

    bin_x += bin_width;
  }

  render_triangle_bin_groups.resize(bins.size());
}

const std::vector<Bin> &Binner::get_bins() const { return bins; }

std::vector<RenderTriangle> &Binner::get_render_bin_group(uint index) {
  return render_triangle_bin_groups[index];
}

} // namespace Archa
