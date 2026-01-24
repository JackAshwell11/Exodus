// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/ecs/component_base.hpp"

namespace exodus::ecs::components {
/// Allows a game object to have a visual representation using a sprite.
struct Sprite final : ComponentBase {
  /// Construct a Sprite component.
  ///
  /// @param texture_id The OpenGL texture ID.
  /// @param width The width of the sprite.
  /// @param height The height of the sprite.
  Sprite(const unsigned int texture_id, const float width, const float height)
      : texture_id(texture_id), width(width), height(height) {}

  /// The OpenGL texture ID.
  unsigned int texture_id;

  /// The width of the sprite.
  float width;

  /// The height of the sprite.
  float height;
};
}  // namespace exodus::ecs::components
