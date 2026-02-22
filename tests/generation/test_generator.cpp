// Std headers
#include <algorithm>
#include <ranges>
#include <unordered_set>

// Local headers
#include "exodus/generation/generator.hpp"
#include "macros.hpp"

namespace exodus::generation {
/// Test that generating a chunk returns an array of the correct size.
TEST(GeneratorTest, GenerateChunkReturnsCorrectSize) {
  const auto tiles{generate_chunk({0, 0}, 42)};
  ASSERT_EQ(tiles.size(), CHUNK_TILE_COUNT);
}

/// Test that generating with the same seed and position produces identical results.
TEST(GeneratorTest, SameSeedAndPositionProducesIdenticalResults) {
  const auto tiles_one{generate_chunk({0, 0}, 42)};
  const auto tiles_two{generate_chunk({0, 0}, 42)};
  ASSERT_EQ(tiles_one, tiles_two);
}

/// Test that the tile distribution is different for each tile type.
TEST(GeneratorTest, TileDistributionProportions) {
  // Generate 100 chunks and count the occurrence of each tile type
  std::unordered_map<TileType, int> counts{};
  constexpr auto chunks{std::views::iota(0, 100) |
                        std::views::transform([](const int val) { return generate_chunk({val, val}, 42); })};
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
}  // namespace exodus::generation
