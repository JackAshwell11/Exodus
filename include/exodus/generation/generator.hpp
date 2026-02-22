// Ensure this file is only included once
#pragma once

// Std headers
#include <array>
#include <cstdint>
#include <unordered_set>

// Local headers
#include "exodus/math.hpp"

namespace exodus::generation {
/// The size of a chunk.
constexpr std::size_t CHUNK_SIZE{64};

/// The total number of tiles in a chunk.
constexpr std::size_t CHUNK_TILE_COUNT{CHUNK_SIZE * CHUNK_SIZE};

/// Stores the different types of tiles available in the game world.
enum class TileType : std::uint8_t {
  Player,    // 0
  Enemy,     // 1
  Grass,     // 2
  Water,     // 3
  Hills,     // 4
  Mountain,  // 5
};

/// A struct to hold the generated data for a chunk.
struct ChunkData {
  /// The types of tiles in the chunk.
  std::array<TileType, CHUNK_TILE_COUNT> tiles;

  /// The positions of enemies in the chunk.
  std::unordered_set<Vec2i> enemy_positions;
};

/// Generate a chunk at a given position with a given seed.
///
/// @param chunk_pos The position of the chunk in chunk coordinates.
/// @param seed The seed for deterministic output.
/// @return A ChunkData object containing the tile types and enemy positions for the generated chunk.
auto generate_chunk(const Vec2i& chunk_pos, int seed) -> ChunkData;
}  // namespace exodus::generation
