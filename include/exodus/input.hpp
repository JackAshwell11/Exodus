// Ensure this file is only included once
#pragma once

namespace exodus::input {
/// Contains the possible input actions.
struct InputState {
  bool move_up{false};
  bool move_down{false};
  bool move_left{false};
  bool move_right{false};
};

/// Polls SDL and updates the input state.
void update();

/// Retrieves the current input state.
///
/// @return The current input state.
[[nodiscard]] auto state() -> const InputState&;
}  // namespace exodus::input
