// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

namespace exodus::ecs::components {
/// Allows a game object to have velocity in 2D space.
struct Velocity final {
  /// Construct a Velocity component.
  ///
  /// @param speed The initial speed of the game object.
  explicit Velocity(const float speed) : direction{0.0F, 0.0F}, speed{speed} {}

  /// The normalised direction of the game object.
  Vec2f direction;

  /// The speed of the game object.
  float speed;
};
}  // namespace exodus::ecs::components
