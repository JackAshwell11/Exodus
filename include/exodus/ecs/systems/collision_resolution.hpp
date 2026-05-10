// Ensure this file is only included once
#pragma once

namespace exodus::ecs {
class Registry;

namespace systems {
/// Handle resolving collider overlaps to prevent game objects from colliding with each other.
///
/// @param registry The registry to operate on.
void collision_resolution_system(Registry& registry);
}  // namespace systems
}  // namespace exodus::ecs
