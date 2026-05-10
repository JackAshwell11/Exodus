// Ensure this file is only included once
#pragma once

namespace exodus::ecs {
class Registry;

namespace systems {
/// Handle moving the enemy towards the player game object.
///
/// @param registry The registry to operate on.
void enemy_movement_system(Registry& registry);
}  // namespace systems
}  // namespace exodus::ecs
