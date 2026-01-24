// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/ecs/system_base.hpp"

namespace exodus::ecs::systems {
/// Handles keyboard input and updates velocity for keyboard-controlled entities.
class InputSystem : public SystemBase {
 public:
  /// Allow using the constructor from SystemBase.
  using SystemBase::SystemBase;

  /// Process update logic for the input system.
  ///
  /// @param delta_time The time interval since the last time the function was called.
  void fixed_update(double delta_time) const override;
};
}  // namespace exodus::ecs::systems
