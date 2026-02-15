// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/movement.hpp"
#include "macros.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/movement.hpp tests.
class MovementFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};
};

/// Test that the movement system does nothing if there are no game objects.
TEST_F(MovementFixture, NoGameObjectsDoesNotMoveAnything) {
  movement_system(registry, 1.0F);
  ASSERT_TRUE(true);
}

/// Test that game objects with only a Transform component are not moved.
TEST_F(MovementFixture, GameObjectWithOnlyTransformNotMoved) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position{0.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}

/// Test that game objects with only a Velocity component are not moved.
TEST_F(MovementFixture, GameObjectWithOnlyVelocityNotMoved) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Velocity>(game_object_id, 10.0F);
  movement_system(registry, 1.0F);
  ASSERT_TRUE(true);
}

/// Test that a single game object with both Transform and Velocity components moves correctly.
TEST_F(MovementFixture, SingleGameObjectMovesNorth) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id, 5.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {1.0F, 0.0F};
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position{5.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}

/// Test that a single game object with both Transform and Velocity components moves correctly in a different direction.
TEST_F(MovementFixture, SingleGameObjectMovesEast) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id, 2.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {0.0F, 1.0F};
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position{0.0F, 2.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}

/// Test that movement works with diagonal directions.
TEST_F(MovementFixture, MovementWithDiagonalDirection) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id, 4.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {0.6F, 0.8F};
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position{2.4F, 3.2F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}

/// Test that delta_time affects movement.
TEST_F(MovementFixture, DeltaTimeAffectsMovement) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id, 3.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {1.0F, 0.0F};
  movement_system(registry, 0.5F);
  constexpr Vec2f expected_position{1.5F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}

/// Test that a zero delta_time results in no movement.
TEST_F(MovementFixture, ZeroDeltaTimeNoMovement) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{10.0F, 20.0F});
  registry.add_component<components::Velocity>(game_object_id, 5.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {1.0F, 0.0F};
  movement_system(registry, 0.0F);
  constexpr Vec2f expected_position{10.0F, 20.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}

/// Test that multiple game objects move independently.
TEST_F(MovementFixture, MultipleGameObjectsMoveIndependently) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<components::Transform>(game_object_id_one, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id_one, 2.0F);
  registry.get_component<components::Velocity>(game_object_id_one).direction = {1.0F, 0.0F};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<components::Transform>(game_object_id_two, Vec2f{5.0F, 5.0F});
  registry.add_component<components::Velocity>(game_object_id_two, 3.0F);
  registry.get_component<components::Velocity>(game_object_id_two).direction = {0.0F, 1.0F};
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position_one{2.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id_one).position, expected_position_one);
  constexpr Vec2f expected_position_two{5.0F, 8.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id_two).position, expected_position_two);
}

/// Test that movement accumulates over multiple calls.
TEST_F(MovementFixture, MovementAccumulatesOverCalls) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {1.0F, 0.0F};
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position_one{1.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position_one);
  movement_system(registry, 1.0F);
  constexpr Vec2f expected_position_two{2.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position_two);
}

/// Test that negative delta_time moves in opposite direction.
TEST_F(MovementFixture, NegativeDeltaTime) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  registry.add_component<components::Velocity>(game_object_id, 2.0F);
  registry.get_component<components::Velocity>(game_object_id).direction = {1.0F, 0.0F};
  movement_system(registry, -1.0F);
  constexpr Vec2f expected_position{-2.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Transform>(game_object_id).position, expected_position);
}
}  // namespace exodus::ecs::systems
