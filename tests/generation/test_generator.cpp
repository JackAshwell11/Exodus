// Std headers
#include <algorithm>
#include <ranges>

// Local headers
#include "exodus/generation/generator.hpp"
#include "macros.hpp"

namespace exodus::generation {
/// Test that generating a chunk returns an array of the correct size.
TEST(GeneratorTest, GenerateChunkReturnsCorrectSize) {
  const auto [tiles, enemy_positions]{generate_chunk({0, 0}, 42)};
  ASSERT_EQ(tiles.size(), CHUNK_TILE_COUNT);
}

/// Test that generating with the same seed and position produces identical results.
TEST(GeneratorTest, SameSeedAndPositionProducesIdenticalResults) {
  const auto [tiles_one, enemy_positions_one]{generate_chunk({0, 0}, 42)};
  const auto [tiles_two, enemy_positions_two]{generate_chunk({0, 0}, 42)};
  ASSERT_EQ(tiles_one, tiles_two);
  ASSERT_EQ(enemy_positions_one, enemy_positions_two);
}

/// Test that the tile distribution is different for each tile type.
TEST(GeneratorTest, TileDistributionProportions) {
  // Generate 100 chunks and count the occurrence of each tile type
  std::unordered_map<TileType, int> counts{};
  constexpr auto chunks{std::views::iota(0, 100) |
                        std::views::transform([](const int val) { return generate_chunk({val, val}, 42).tiles; })};
  std::ranges::for_each(chunks | std::views::join, [&](const TileType& tile) { counts[tile]++; });

  // Check that the proportions of the different tiles are different
  std::unordered_map<TileType, float> ratios{};
  std::unordered_set<float> unique_ratios{};
  std::ranges::for_each(counts, [&](const auto& pair) {
    const float ratio{static_cast<float>(pair.second) / (100.0F * CHUNK_TILE_COUNT)};
    ratios[pair.first] = ratio;
    unique_ratios.insert(ratio);
  });
  ASSERT_EQ(unique_ratios.size(), ratios.size());
}

/// Test that enemies are only placed on allowed tile types.
TEST(GenerationTest, EnemiesOnlyAllowedTiles) {
  const auto [tiles, enemy_positions]{generate_chunk({0, 0}, 42)};
  for (const auto& enemy_position : enemy_positions) {
    const TileType tile_type{tiles.at((enemy_position.y * CHUNK_SIZE) + enemy_position.x)};
    ASSERT_TRUE(tile_type == TileType::Grass || tile_type == TileType::Hills || tile_type == TileType::Mountain);
  }
}

/// Test that some chunks contain enemies and some do not.
TEST(GenerationTest, EnemyDensityStatisticalCheck) {
  constexpr int total_chunks{100};
  constexpr auto empty_flags{std::views::iota(0, total_chunks) | std::views::transform([](const int i) {
                               return generate_chunk({i, i}, 42).enemy_positions.empty();
                             })};
  const int chunks_without_enemies{static_cast<int>(std::ranges::count(empty_flags, true))};
  const int chunks_with_enemies{total_chunks - chunks_without_enemies};
  ASSERT_GT(chunks_with_enemies, 0);
  ASSERT_GT(chunks_without_enemies, 0);
  ASSERT_LT(chunks_with_enemies, total_chunks);
}
}  // namespace exodus::generation
