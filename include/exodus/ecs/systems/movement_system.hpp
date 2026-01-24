// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/ecs/system_base.hpp"

namespace exodus::ecs::systems {
/// Handles movement of game objects.
struct MovementSystem : SystemBase {
  /// Allow using the constructor from SystemBase.
  using SystemBase::SystemBase;

  /// Process update logic for the movement system.
  ///
  /// @param delta_time The time interval since the last time the function was called.
  void fixed_update(double delta_time) const override;
};
}  // namespace exodus::ecs::systems
