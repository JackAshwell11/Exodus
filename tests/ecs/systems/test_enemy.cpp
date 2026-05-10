// Local headers
#include "exodus/ecs/components/enemy.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/enemy.hpp"
#include "macros.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/enemy.hpp tests.
class EnemyMovementFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};
};

/// Test that enemies are not updated when there are no players.
TEST_F(EnemyMovementFixture, NoPlayersDoesNotChangeEnemyVelocity) {
  const GameObjectID enemy_id{registry.create()};
  registry.add_component<components::Enemy>(enemy_id);
  registry.add_component<components::Transform>(enemy_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(enemy_id, 1.0F);
  registry.get_component<components::Velocity>(enemy_id).direction = {1.0F, 0.0F};
  enemy_movement_system(registry);
  constexpr Vec2f expected_direction{1.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(enemy_id).direction, expected_direction);
}

/// Test that enemies missing required components are ignored.
TEST_F(EnemyMovementFixture, IgnoresEnemiesWithoutRequiredComponents) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{0.0F, 0.0F});
  const GameObjectID enemy_id{registry.create()};
  registry.add_component<components::Enemy>(enemy_id);
  registry.add_component<components::Velocity>(enemy_id, 1.0F);
  registry.get_component<components::Velocity>(enemy_id).direction = {0.5F, 0.5F};
  enemy_movement_system(registry);
  constexpr Vec2f expected_direction{0.5F, 0.5F};
  ASSERT_EQ(registry.get_component<components::Velocity>(enemy_id).direction, expected_direction);
}

/// Test that a single enemy seeks the player.
TEST_F(EnemyMovementFixture, SingleEnemySeeksPlayer) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{10.0F, 20.0F});
  const GameObjectID enemy_id{registry.create()};
  registry.add_component<components::Enemy>(enemy_id);
  registry.add_component<components::Transform>(enemy_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(enemy_id, 1.0F);
  enemy_movement_system(registry);
  const Vec2f expected_direction{Vec2f{10.0F, 20.0F}.normalise()};
  const  Vec2f& direction{registry.get_component<components::Velocity>(enemy_id).direction};
  ASSERT_NEAR(direction.x, expected_direction.x, 1e-5F);
  ASSERT_NEAR(direction.y, expected_direction.y, 1e-5F);
}

/// Test that an enemy at the player position has a zero direction.
TEST_F(EnemyMovementFixture, EnemyAtPlayerPositionSetsZeroDirection) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{5.0F, 5.0F});
  const GameObjectID enemy_id{registry.create()};
  registry.add_component<components::Enemy>(enemy_id);
  registry.add_component<components::Transform>(enemy_id, Vec2f{5.0F, 5.0F});
  registry.add_component<components::Velocity>(enemy_id, 1.0F);
  enemy_movement_system(registry);
  constexpr Vec2f expected_direction{0.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(enemy_id).direction, expected_direction);
}
}  // namespace exodus::ecs::systems
