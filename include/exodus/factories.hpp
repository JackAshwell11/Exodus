// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

// Forward declare to avoid bringing in OpenGL headers
using GLuint = unsigned int;

namespace exodus {
namespace ecs {
class Registry;
}  // namespace ecs

namespace generation {
enum class TileType : std::uint8_t;
}  // namespace generation

/// Get the mapping of tile types to OpenGL texture IDs for the game objects.
///
/// @return A reference to the mapping of tile types to OpenGL texture IDs for the game objects.
auto get_tile_textures() -> std::unordered_map<generation::TileType, GLuint>&;

/// Create a game object if possible.
///
/// @param registry The registry to create the game object in.
/// @param tile_type The type of tile to create the game object for.
/// @param position The position to create the game object at.
void create_game_object(ecs::Registry& registry, const generation::TileType& tile_type, const Vec2f& position);
}  // namespace exodus
