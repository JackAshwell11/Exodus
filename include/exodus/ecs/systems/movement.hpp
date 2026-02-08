// Ensure this file is only included once
#pragma once

namespace exodus::ecs {
class Registry;

namespace systems {
/// Handle computing game object movement based on position and velocity.
///
/// @param registry The registry to operate on.
/// @param delta_time The time interval since the last time the function was called.
void movement_system(Registry& registry, float delta_time);
}  // namespace systems
}  // namespace exodus::ecs
