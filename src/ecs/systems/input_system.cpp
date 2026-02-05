// Related header
#include "exodus/ecs/systems/input_system.hpp"

// Local headers
#include "exodus/ecs/components/keyboard_controlled.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/input.hpp"
#include "exodus/math.hpp"

namespace exodus::ecs::systems {
void InputSystem::fixed_update(const double /*delta_time*/) const {
  // Process all entities with KeyboardControlled and Velocity components
  for (const auto& [id, components] :
       registry_->get_game_object_components<components::KeyboardControlled, components::Velocity>()) {
    auto& [keyboard, velocity] = components;

    // Read keyboard input and calculate direction
    Vec2f input_direction{0.0F, 0.0F};
    if (input::state().move_up) {
      input_direction.y -= 1.0F;
    }
    if (input::state().move_down) {
      input_direction.y += 1.0F;
    }
    if (input::state().move_left) {
      input_direction.x -= 1.0F;
    }
    if (input::state().move_right) {
      input_direction.x += 1.0F;
    }

    // Update the velocity with a normalised direction
    if (const Vec2f normalised_direction = input_direction.normalise(); normalised_direction != Vec2f{0.0f, 0.0f}) {
      // Update velocity based on keyboard input
      velocity->direction = normalised_direction;
    } else {
      // No input - stop moving
      velocity->direction = {0.0F, 0.0F};
    }
  }
}
}  // namespace exodus::ecs::systems
