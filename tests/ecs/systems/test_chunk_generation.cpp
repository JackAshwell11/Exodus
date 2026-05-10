// Local headers
#include "exodus/ecs/components/enemy.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/chunk_generation.hpp"
#include "exodus/generation/generator.hpp"
#include "macros.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/chunk_generation.hpp tests.
class ChunkGenerationFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};

  /// Set up the fixture for the tests.
  void SetUp() override { get_generated_chunks().clear(); }
};

/// Test that the chunk generation system does nothing if there are no players.
TEST_F(ChunkGenerationFixture, NoPlayersDoesNotGenerateChunks) {
  chunk_generation_system(registry);
  ASSERT_EQ(registry.count(), 0);
}

/// Test that the chunk generation system generates chunks around the player.
TEST_F(ChunkGenerationFixture, GeneratesChunksAroundPlayer) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{0.0F, 0.0F});
  chunk_generation_system(registry);

  // Chunk generation has a radius of 2 so 5x5 chunks each with CHUNK_SIZE*CHUNK_SIZE (4096) tiles
  constexpr std::size_t expected_new_game_objects{102400};     // The actual number is larger due to enemy counts
  ASSERT_GT(registry.count(), 1 + expected_new_game_objects);  // Player is created too
}

/// Test that already generated chunks are not regenerated.
TEST_F(ChunkGenerationFixture, DoesNotRegenerateAlreadyGeneratedChunks) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{0.0F, 0.0F});
  chunk_generation_system(registry);
  const std::size_t size_after_first{registry.count()};
  chunk_generation_system(registry);
  ASSERT_EQ(registry.count(), size_after_first);
}

/// Test that moving the player generates new chunks.
TEST_F(ChunkGenerationFixture, MovingPlayerGeneratesNewChunks) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{0.0F, 0.0F});
  chunk_generation_system(registry);
  const std::size_t size_after_first{registry.count()};

  // Move the player far enough away so they reach ungenerated chunks
  registry.get_component<components::Transform>(player_id).position = {100.0F * generation::CHUNK_SIZE, 0.0F};
  chunk_generation_system(registry);

  // Should generate a 5x5 grid of chunks each with CHUNK_SIZE*CHUNK_SIZE (4096) tiles
  constexpr std::size_t expected_new_game_objects{102400};  // The actual number is larger due to enemy counts
  ASSERT_GT(registry.count(), size_after_first + expected_new_game_objects);
}

/// Test that multiple players are handled (this should realistically never happen).
TEST_F(ChunkGenerationFixture, MultiplePlayers) {
  const GameObjectID player_id_one{registry.create()};
  registry.add_component<components::Player>(player_id_one);
  registry.add_component<components::Transform>(player_id_one, Vec2f{0.0F, 0.0F});
  const GameObjectID player_id_two{registry.create()};
  registry.add_component<components::Player>(player_id_two);
  registry.add_component<components::Transform>(player_id_two, Vec2f{100.0F * generation::CHUNK_SIZE, 0.0F});
  chunk_generation_system(registry);

  // Should generate two 5x5 grids of chunks each with CHUNK_SIZE*CHUNK_SIZE (4096) tiles
  constexpr std::size_t expected_new_game_objects{204800};     // The actual number is larger due to enemy counts
  ASSERT_GT(registry.count(), 2 + expected_new_game_objects);  // Both players are created too
}

/// Test that chunks are generated for negative player positions.
TEST_F(ChunkGenerationFixture, GeneratesChunksAtNegativePositions) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{-100.0F, -100.0F});
  chunk_generation_system(registry);

  // Chunk generation has a radius of 2 so 5x5 chunks each with CHUNK_SIZE*CHUNK_SIZE (4096) tiles
  constexpr std::size_t expected_new_game_objects{102400};  // The actual number is larger due to enemy counts
  ASSERT_GT(registry.count(), 1 + expected_new_game_objects);
}

/// Test that tile positions in negative chunks are correctly grid-aligned.
TEST_F(ChunkGenerationFixture, NegativeChunkTilePositionsAreGridAligned) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{-100.0F, -100.0F});
  chunk_generation_system(registry);

  // Verify every generated tile has integer-aligned positions
  for (const auto& [transform] : registry.view<components::Transform>()) {
    ASSERT_NEAR(transform.position.x - std::floor(transform.position.x), 0.0F, 1e-5F);
    ASSERT_NEAR(transform.position.y - std::floor(transform.position.y), 0.0F, 1e-5F);
  }
}

/// Test that enemy game objects are created by the chunk generation system.
TEST_F(ChunkGenerationFixture, CreatesEnemyGameObjects) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{0.0F, 0.0F});
  chunk_generation_system(registry);
  constexpr std::size_t expected_enemy_count{697};
  ASSERT_EQ(std::ranges::distance(registry.view<components::Enemy>()), expected_enemy_count);
}
}  // namespace exodus::ecs::systems
