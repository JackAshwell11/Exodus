// Related header
#include "exodus/ecs/systems/input.hpp"

// Local headers
#include "exodus/ecs/components/keyboard_controlled.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"

namespace exodus {
namespace {
/// The current input state.
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
InputState current;
}  // namespace

auto input_state() -> InputState& { return current; }

namespace ecs::systems {
void input_system(Registry& registry) {
  for (const auto& [keyboard_controlled, velocity] :
       registry.view<components::KeyboardControlled, components::Velocity>()) {
    // Read keyboard input and calculate direction
    Vec2f input_direction{0.0F, 0.0F};
    if (current.move_up) {
      input_direction.y -= 1.0F;
    }
    if (current.move_down) {
      input_direction.y += 1.0F;
    }
    if (current.move_left) {
      input_direction.x -= 1.0F;
    }
    if (current.move_right) {
      input_direction.x += 1.0F;
    }

    // Update the velocity with a normalised direction
    if (const Vec2f normalised_direction{input_direction.normalise()}; normalised_direction != Vec2f{0.0F, 0.0F}) {
      // Update velocity based on keyboard input
      velocity.direction = normalised_direction;
    } else {
      // No input - stop moving
      velocity.direction = {0.0F, 0.0F};
    }
  }
}
}  // namespace ecs::systems
}  // namespace exodus
