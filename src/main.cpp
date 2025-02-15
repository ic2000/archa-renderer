#include "game.hpp"

#include "config.hpp"
#include "logger.hpp"

using namespace Archa;

int main() {
  Logger().info() << "Starting Archa Engine" << '\n';

#ifdef USING_SIMD_AVX2
  Logger().info() << "Using AVX2 SIMD (32-byte alignment)" << '\n';
#elif defined USING_SIMD_SSE2
  Logger().info() << "Using SSE2 SIMD (16-byte alignment)" << '\n';
#else
  Logger().info() << "No SIMD" << '\n';
#endif

  Game game{};
  game.init({1280, 720}, "Archa Engine", static_cast<float>(1) / 1);
  // game.init({1281, 720}, "Archa Engine", static_cast<float>(1) / 1);
  game.run();

  return 0;
}
