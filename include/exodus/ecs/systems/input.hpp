// Ensure this file is only included once
#pragma once

namespace exodus {
/// Contains the possible input actions.
struct InputState {
  /// Whether the player is moving up.
  bool move_up{false};

  /// Whether the player is moving down.
  bool move_down{false};

  /// Whether the player is moving left.
  bool move_left{false};

  /// Whether the player is moving right.
  bool move_right{false};
};

/// Retrieves the current input state.
///
/// @return The current input state.
[[nodiscard]] auto input_state() -> InputState&;

namespace ecs {
class Registry;

namespace systems {
/// Handle keyboard input and updates velocity for keyboard-controlled game objects.
///
/// @param registry The registry to operate on.
void input_system(Registry& registry);
}  // namespace systems
}  // namespace ecs
}  // namespace exodus
