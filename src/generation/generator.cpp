// Related header
#include "exodus/generation/generator.hpp"

// Local headers
#include "exodus/generation/noise.hpp"

namespace {
/// Controls the scale of the terrain features.
/// Lower values (e.g. 0.01) result in large, sweeping features like continents.
/// Higher values (e.g. 0.1) result in smaller, more frequent features like jagged hills.
constexpr double NOISE_FREQUENCY{0.025};

/// Defines the distribution of tile types based on noise values.
constexpr std::array<std::pair<float, exodus::generation::TileType>, 4> TILE_DISTRIBUTION{{
    {0.5F, exodus::generation::TileType::Grass},      // 50% of the terrain
    {0.20F, exodus::generation::TileType::Water},     // 20% of the terrain
    {0.10F, exodus::generation::TileType::Sand},      // 10% of the terrain
    {0.20F, exodus::generation::TileType::Mountain},  // 20% of the terrain
}};

/// Maps an OpenSimplex2S noise value to a TileType based on predefined thresholds.
///
/// @param noise The noise value.
/// @return The corresponding TileType.
constexpr auto tile_from_noise(const float noise) -> exodus::generation::TileType {
  float cumulative_threshold{0.0F};
  const float mapped{(noise + 1.0F) * 0.5F};
  for (const auto& [weight, tile] : TILE_DISTRIBUTION) {
    cumulative_threshold += weight;
    if (mapped <= cumulative_threshold) {
      return tile;
    }
  }
  return TILE_DISTRIBUTION.back().second;
}
}  // namespace

namespace exodus::generation {
auto generate_chunk(const Vec2i chunk_pos, const int seed) -> std::array<TileType, CHUNK_TILE_COUNT> {
  std::array<TileType, CHUNK_TILE_COUNT> tiles{};
  const Vec2d chunk_origin{chunk_pos * CHUNK_SIZE};
  for (std::size_t y{0}; y < CHUNK_SIZE; y++) {
    for (std::size_t x{0}; x < CHUNK_SIZE; x++) {
      const Vec2d sample_pos{chunk_origin + Vec2d{static_cast<double>(x), static_cast<double>(y)}};
      const float generated_noise{noise(seed, sample_pos * NOISE_FREQUENCY)};
      tiles.at((y * CHUNK_SIZE) + x) = tile_from_noise(generated_noise);
    }
  }
  return tiles;
}
}  // namespace exodus::generation
