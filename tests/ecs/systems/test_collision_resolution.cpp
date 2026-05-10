// Local headers
#include "exodus/ecs/components/collider.hpp"
#include "exodus/ecs/components/enemy.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/collision_resolution.hpp"
#include "exodus/world_config.hpp"
#include "macros.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/collision_resolution.hpp tests.
class CollisionResolutionFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};
};

/// Test that collision resolution does nothing if there is no player.
TEST_F(CollisionResolutionFixture, NoPlayerDoesNotMoveEnemies) {
  const GameObjectID enemy_id_one{registry.create()};
  registry.add_component<components::Enemy>(enemy_id_one);
  registry.add_component<components::Transform>(enemy_id_one, Vec2f{0.0F, 0.0F});
  registry.add_component<components::CircleCollider>(enemy_id_one, 10.0F);
  const GameObjectID enemy_id_two{registry.create()};
  registry.add_component<components::Enemy>(enemy_id_two);
  registry.add_component<components::Transform>(enemy_id_two, Vec2f{5.0F, 0.0F});
  registry.add_component<components::CircleCollider>(enemy_id_two, 10.0F);
  collision_resolution_system(registry);
  constexpr Vec2f expected_position_one{0.0F, 0.0F};
  constexpr Vec2f expected_position_two{5.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(enemy_id_one).position, expected_position_one);
  ASSERT_EQ(registry.get_component<components::Transform>(enemy_id_two).position, expected_position_two);
}

/// Test that non-overlapping enemies are not moved.
TEST_F(CollisionResolutionFixture, NonOverlappingEnemiesNotMoved) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{1000.0F, 0.0F});
  const GameObjectID enemy_id_one{registry.create()};
  registry.add_component<components::Enemy>(enemy_id_one);
  registry.add_component<components::Transform>(enemy_id_one, Vec2f{0.0F, 0.0F});
  registry.add_component<components::CircleCollider>(enemy_id_one, 10.0F);
  const GameObjectID enemy_id_two{registry.create()};
  registry.add_component<components::Enemy>(enemy_id_two);
  registry.add_component<components::Transform>(enemy_id_two, Vec2f{20.0F, 0.0F});
  registry.add_component<components::CircleCollider>(enemy_id_two, 10.0F);
  collision_resolution_system(registry);
  constexpr Vec2f expected_position_one{0.0F, 0.0F};
  constexpr Vec2f expected_position_two{20.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(enemy_id_one).position, expected_position_one);
  ASSERT_EQ(registry.get_component<components::Transform>(enemy_id_two).position, expected_position_two);
}

/// Test that overlapping enemies are separated symmetrically.
TEST_F(CollisionResolutionFixture, OverlappingEnemiesSeparatedSymmetrically) {
  const GameObjectID player_id{registry.create()};
  registry.add_component<components::Player>(player_id);
  registry.add_component<components::Transform>(player_id, Vec2f{1000.0F, 0.0F});
  const GameObjectID enemy_id_one{registry.create()};
  registry.add_component<components::Enemy>(enemy_id_one);
  registry.add_component<components::Transform>(enemy_id_one, Vec2f{0.0F, 0.0F});
  registry.add_component<components::CircleCollider>(enemy_id_one, 10.0F);
  const GameObjectID enemy_id_two{registry.create()};
  registry.add_component<components::Enemy>(enemy_id_two);
  registry.add_component<components::Transform>(enemy_id_two, Vec2f{5.0F, 0.0F});
  registry.add_component<components::CircleCollider>(enemy_id_two, 10.0F);
  collision_resolution_system(registry);
  const Vec2f& position_one{registry.get_component<components::Transform>(enemy_id_one).position};
  const Vec2f& position_two{registry.get_component<components::Transform>(enemy_id_two).position};
  ASSERT_NEAR(position_one.x, -0.25F, 1e-5F);
  ASSERT_NEAR(position_one.y, 0.0F, 1e-5F);
  ASSERT_NEAR(position_two.x, 5.25F, 1e-5F);
  ASSERT_NEAR(position_two.y, 0.0F, 1e-5F);
}
}  // namespace exodus::ecs::systems
