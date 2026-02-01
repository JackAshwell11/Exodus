// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/game_object.hpp"
#include "exodus/math.hpp"

class Registry;

namespace exodus {
namespace generation {
enum class TileType : std::uint8_t;
}  // namespace generation

/// Create a game object if possible.
///
/// @param registry The registry to create the game object in.
/// @param tile_type The type of tile to create the game object for.
/// @param position The position to create the game object at.
/// @return The ID of the created game object.
auto create_game_object(Registry* registry, generation::TileType tile_type, const Vec2f& position)
    -> GameObjectID;
}  // namespace exodus
