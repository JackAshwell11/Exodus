// Ensure this file is only included once
#pragma once

// Std headers
#include <array>
#include <cstdint>

// Local headers
#include "exodus/math.hpp"

namespace exodus::generation {
/// The size of a chunk.
constexpr std::size_t CHUNK_SIZE{64};

/// The total number of tiles in a chunk.
constexpr std::size_t CHUNK_TILE_COUNT{CHUNK_SIZE * CHUNK_SIZE};

/// Stores the different types of tiles available in the game world.
enum class TileType : std::uint8_t {
  Grass,     // 0
  Water,     // 1
  Sand,      // 2
  Mountain,  // 3
};

/// Generate a chunk at a given position with a given seed.
///
/// @param chunk_pos The position of the chunk in chunk coordinates.
/// @param seed The seed for the random number generator.
/// @return A vector of TileType representing the generated chunk.
auto generate_chunk(Vec2i chunk_pos, int seed) -> std::array<TileType, CHUNK_TILE_COUNT>;
}  // namespace exodus::generation
