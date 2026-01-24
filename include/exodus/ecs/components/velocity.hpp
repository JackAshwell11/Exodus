// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/ecs/component_base.hpp"
#include "exodus/math.hpp"

namespace exodus::ecs::components {
/// Allows a game object to have velocity in 2D space.
struct Velocity final : ComponentBase {
  /// Construct a Velocity component.
  ///
  /// @param speed The initial speed of the game object.
  explicit Velocity(const float speed) : direction{0.0f, 0.0f}, speed{speed} {}

  /// The normalised direction of the game object.
  Vec2f direction;

  /// The speed of the game object.
  float speed;
};
}  // namespace exodus::ecs::components
