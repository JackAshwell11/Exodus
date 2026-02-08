// Ensure this file is only included once
#pragma once

namespace exodus::ecs::components {
/// Allows a game object to have a visual representation using a sprite.
struct Sprite final {
  /// Construct a Sprite component.
  ///
  /// @param texture_id The OpenGL texture ID.
  /// @param depth The Z-depth of the sprite for rendering order.
  /// @param scale The scale factor to apply to the sprite.
  explicit Sprite(const unsigned int texture_id, const int depth, const float scale = 1.0F)
      : texture_id(texture_id), depth(depth), scale(scale) {}

  /// The OpenGL texture ID.
  unsigned int texture_id;

  /// The Z-depth of the sprite for rendering order.
  int depth;

  /// The scale factor to apply to the sprite.
  float scale;
};
}  // namespace exodus::ecs::components
