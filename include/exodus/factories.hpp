// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

namespace exodus {
namespace ecs {
class Registry;
}  // namespace ecs

namespace generation {
enum class TileType : std::uint8_t;
}  // namespace generation

/// Create a game object if possible.
///
/// @param registry The registry to create the game object in.
/// @param tile_type The type of tile to create the game object for.
/// @param position The position to create the game object at.
void create_game_object(ecs::Registry& registry, const generation::TileType& tile_type, const Vec2f& position);
}  // namespace exodus
