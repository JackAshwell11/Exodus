// Ensure this file is only included once
#pragma once

// Std headers
#include <unordered_set>

// Local headers
#include "exodus/math.hpp"

namespace exodus::ecs {
class Registry;

namespace systems {
/// Get the set of chunks which have already been generated.
///
/// @return Reference to the set of generated chunks.
auto get_generated_chunks() -> std::unordered_set<Vec2i>&;

/// Handle generating chunks of the world around the player as they move through the world.
///
/// @param registry The registry to operate on.
void chunk_generation_system(Registry& registry);
}  // namespace systems
}  // namespace exodus::ecs
