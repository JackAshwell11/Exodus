// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/ecs/component_base.hpp"
#include "exodus/math.hpp"

namespace exodus::ecs::components {
/// Allows a game object to have a position in 2D space.
struct Transform final : ComponentBase {
  /// Construct a Transform component.
  ///
  /// @param position The initial position of the game object.
  explicit Transform(const Vec2f& position) : ComponentBase(), position(position) {}

  /// The position of the game object in 2D space.
  Vec2f position;
};
}  // namespace exodus::ecs::components
