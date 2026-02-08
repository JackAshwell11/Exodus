// Related header
#include "exodus/input.hpp"

// Std headers
#include <span>

// External headers
#include <SDL_keyboard.h>

namespace {
/// Check if a key is currently held down.
///
/// @param keys The current keyboard state.
/// @param scancode The scancode of the key to check.
/// @return True if the key is held down, false otherwise.
[[nodiscard]] auto is_key_down(const std::span<const Uint8> keys, const SDL_Scancode scancode) -> bool {
  const auto idx = static_cast<size_t>(scancode);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  return idx < keys.size() && keys[idx] != 0;
}
}  // namespace

namespace exodus::input {
namespace {
/// The current input state.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
InputState current;
}  // namespace

void update() {
  // Get the currently pressed keys
  int key_count = 0;
  const Uint8* raw = SDL_GetKeyboardState(&key_count);
  if (raw == nullptr) {
    return;
  }

  // Determine what keys are pressed
  const std::span keys{raw, static_cast<size_t>(key_count)};
  current.move_up = is_key_down(keys, SDL_SCANCODE_W);
  current.move_down = is_key_down(keys, SDL_SCANCODE_S);
  current.move_left = is_key_down(keys, SDL_SCANCODE_A);
  current.move_right = is_key_down(keys, SDL_SCANCODE_D);
}

auto state() -> const InputState& { return current; }
}  // namespace exodus::input
