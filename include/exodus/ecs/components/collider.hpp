// Ensure this file is only included once
#pragma once

namespace exodus::ecs::components {
/// Allows a game object to have a simple circle collider.
struct CircleCollider final {
  /// Construct a circle collider.
  ///
  /// @param radius The circle's radius
  explicit CircleCollider(const float radius)
    : radius{radius} {}

  /// The radius of the circle
  float radius;
};
}  // namespace exodus::ecs::components

