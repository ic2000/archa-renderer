#include "config.hpp"

#include "game.hpp"

#include "intrinsics.hpp"

using namespace Archa;

int main() {
  Game game{};
  game.init({1280, 720}, "Archa Engine", static_cast<float>(1) / 1);
  game.run();

  // auto test = Test<SSE2::Functions>();

  return 0;
}
