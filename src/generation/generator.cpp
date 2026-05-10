// Related header
#include "exodus/generation/generator.hpp"

// Std headers
#include <algorithm>

// Local headers
#include "exodus/generation/noise.hpp"

namespace exodus::generation {
namespace {
// ----- Frequencies to control the scale of different noise layers -----

/// Controls the scale of continent-sized terrain features.
constexpr double CONTINENT_FREQUENCY{0.0008};

/// Controls the scale of local terrain variation such as hills and ridges.
constexpr double DETAIL_FREQUENCY{0.0025};

/// Controls the spread of enemies.
constexpr double ENEMY_FREQUENCY{0.002};

// ----- Weights to control how much each noise layer contributes to final terrain elevation -----

/// Controls how strongly continent noise influences final terrain elevation.
constexpr float CONTINENT_WEIGHT{0.6F};

/// Controls how strongly detail noise influences final terrain elevation.
constexpr float DETAIL_WEIGHT{0.4F};

// ----- Fractal Brownian Motion (fBm) settings to control terrain/detail complexity -----

/// Controls the number of noise layers combined for fBm.
constexpr int FBM_OCTAVES{4};

/// Controls how much each successive octave contributes to the fBm output.
constexpr float FBM_PERSISTENCE{0.5F};

/// Controls how much the frequency increases for each successive octave.
constexpr double FBM_LACUNARITY{2.0};

// ----- Thresholds for mapping noise values to TileType -----

/// Threshold below which terrain is considered water.
constexpr float WATER_THRESHOLD{0.35F};

/// Threshold below which terrain is considered grass.
constexpr float GRASS_THRESHOLD{0.75F};

/// Threshold below which terrain is considered hills.
constexpr float HILLS_THRESHOLD{0.85F};

/// Threshold below which a tile may spawn an enemy.
constexpr float ENEMY_THRESHOLD{0.25F};

// ----- The global biases to influence the overall tile distribution -----

/// Controls the global bias toward land versus water.
constexpr float LAND_BIAS{0.08F};

// ----- The OpenSimplex2S noise output range -----

/// The scale used to map noise from [-1, 1] to [0, 1].
constexpr float NOISE_MAP_SCALE{0.5F};

/// The minimum expected output value of OpenSimplex2S noise.
constexpr float OPENSIMPLEX2S_MIN{-1.0F};

/// The maximum expected output value of OpenSimplex2S noise.
constexpr float OPENSIMPLEX2S_MAX{1.0F};

// ----- Allowed tile types for placing game objects -----

/// The set of tile types that are valid for enemy placement.
constexpr std::array ALLOWED_ENEMY_TILES{TileType::Grass, TileType::Hills, TileType::Mountain};

/// Computes fractal Brownian motion (fBm) noise by combining multiple octaves of OpenSimplex2S noise.
///
/// fBm (fractional Brownian motion) is a technique for summing multiple scaled noise layers to
/// produce more natural-looking, varied output (see https://en.wikipedia.org/wiki/Fractional_Brownian_motion).
///
/// @param pos The 2D position at which to evaluate the noise.
/// @param seed The seed for deterministic output.
/// @return A noise value that combines multiple octaves of noise to create more complex terrain features.
auto fbm_noise(const Vec2d& pos, const int seed) -> float {
  float value{0.0F};
  float amplitude{1.0F};
  double frequency{1.0};
  float normalisation{0.0F};
  for (int octave{0}; octave < FBM_OCTAVES; octave++) {
    value += amplitude * noise(pos * frequency, seed);
    normalisation += amplitude;
    amplitude *= FBM_PERSISTENCE;
    frequency *= FBM_LACUNARITY;
  }
  return value / normalisation;
}

/// Maps a noise value from [-1,1] to [0,1].
///
/// @param noise The noise value to map (expected to be in the range [-1.0, 1.0])
/// @return The mapped noise value in the range [0.0, 1.0]
constexpr auto map_noise(const float noise) -> float { return (noise + 1.0F) * NOISE_MAP_SCALE; }

/// Maps a noise elevation value to a TileType based on predefined thresholds.
///
/// @param elevation The noise elevation value (in the range [-1.0, 1.0])
/// @return The corresponding TileType based on the elevation thresholds.
auto tile_from_elevation(const float elevation) -> TileType {
  const float mapped{map_noise(elevation)};
  if (mapped < WATER_THRESHOLD) {
    return TileType::Water;
  }
  if (mapped < GRASS_THRESHOLD) {
    return TileType::Grass;
  }
  if (mapped < HILLS_THRESHOLD) {
    return TileType::Hills;
  }
  return TileType::Mountain;
}

/// Determines if an enemy can spawn on a given tile type based on the allowed enemy tiles.
///
/// @param tile The TileType to check for enemy spawning.
/// @return True if an enemy can spawn on the tile, false otherwise.
constexpr auto can_spawn_enemy(const TileType& tile) -> bool {
  return std::ranges::find(ALLOWED_ENEMY_TILES, tile) != ALLOWED_ENEMY_TILES.end();
}
}  // namespace

auto generate_chunk(const Vec2i& chunk_pos, const int seed) -> ChunkData {
  ChunkData chunk{};
  const Vec2i chunk_origin{chunk_pos * CHUNK_SIZE};
  for (std::size_t y{0}; y < CHUNK_SIZE; y++) {
    for (std::size_t x{0}; x < CHUNK_SIZE; x++) {
      const Vec2d world_pos{chunk_origin + Vec2d{static_cast<double>(x), static_cast<double>(y)}};

      // Perform the terrain pass to determine the elevation of the tile
      const float continent{noise(world_pos * CONTINENT_FREQUENCY, seed)};
      const float detail{fbm_noise(world_pos * DETAIL_FREQUENCY, seed)};
      const float elevation{std::clamp(continent * CONTINENT_WEIGHT + detail * DETAIL_WEIGHT + LAND_BIAS,
                                       OPENSIMPLEX2S_MIN, OPENSIMPLEX2S_MAX)};
      const TileType terrain_tile{tile_from_elevation(elevation)};
      chunk.tiles.at((y * CHUNK_SIZE) + x) = terrain_tile;

      // Perform the enemy pass to determine if an enemy should be placed on this tile
      if (can_spawn_enemy(terrain_tile) && map_noise(fbm_noise(world_pos * ENEMY_FREQUENCY, seed)) < ENEMY_THRESHOLD) {
        chunk.enemy_positions.emplace(static_cast<int>(x), static_cast<int>(y));
      }
    }
  }
  return chunk;
}
}  // namespace exodus::generation
