// Ensure this file is only included once
#pragma once

// Local headers
#include "math.hpp"

namespace exodus {
/// Holds configuration values related to the world.
struct WorldConfig {
  /// The base size of a sprite texture.
  static constexpr int SPRITE_TEXTURE_SIZE{128};

  /// The scale factor to apply to sprites.
  static constexpr float SPRITE_SCALE{0.25F};

  /// The final size of a sprite in pixels after scaling.
  static constexpr float SPRITE_SIZE{SPRITE_TEXTURE_SIZE * SPRITE_SCALE};

  /// The size of the world in pixels.
  static constexpr Vec2f SCREEN_SIZE{1280.0F, 720.0F};
};
}  // namespace exodus
