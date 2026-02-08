// Ensure this file is only included once
#pragma once

namespace exodus::ecs {
class Registry;

namespace systems {
/// Handle generating chunks of the world around the player as they move through the world.
///
/// @param registry The registry to operate on.
void chunk_generation_system(Registry& registry);
}  // namespace systems
}  // namespace exodus::ecs
