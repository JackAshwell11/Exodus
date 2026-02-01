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
  explicit Sprite(const unsigned int texture_id) : texture_id(texture_id) {}

  /// The OpenGL texture ID.
  unsigned int texture_id;
};
}  // namespace exodus::ecs::components
