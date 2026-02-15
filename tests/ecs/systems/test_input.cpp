// Local headers
#include "exodus/ecs/components/keyboard_controlled.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/input.hpp"
#include "macros.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/input.hpp tests.
class InputFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};

  /// Set up the fixture for the tests.
  void SetUp() override { input_state() = InputState{}; }
};

/// Test that the input system does nothing if there are no game objects.
TEST_F(InputFixture, NoGameObjectsDoesNotAffectAnything) {
  input_system(registry);
  ASSERT_TRUE(true);
}

/// Test that game objects without a KeyboardControlled component are not affected.
TEST_F(InputFixture, GameObjectWithoutKeyboardControlledNotAffected) {
  input_state().move_up = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  constexpr Vec2f expected_direction{0.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that game objects without a Velocity component are not affected .
TEST_F(InputFixture, GameObjectsWithoutVelocityNotAffected) {
  input_state().move_up = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  input_system(registry);
  ASSERT_TRUE(true);
}

/// Test that no input sets the direction to zero.
TEST_F(InputFixture, NoInputSetsDirectionToZero) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 5.0F);
  input_system(registry);
  constexpr Vec2f expected_velocity{0.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_velocity);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 5.0F);
}

/// Test that moving upwards sets the direction to up.
TEST_F(InputFixture, MoveUpSetsDirectionUp) {
  input_state().move_up = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  constexpr Vec2f expected_velocity{0.0F, -1.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_velocity);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that moving downwards sets the direction to down.
TEST_F(InputFixture, MoveDownSetsDirectionDown) {
  input_state().move_down = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  constexpr Vec2f expected_velocity{0.0F, 1.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_velocity);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that moving leftwards sets the direction to left.
TEST_F(InputFixture, MoveLeftSetsDirectionLeft) {
  input_state().move_left = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  constexpr Vec2f expected_direction{-1.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that moving rightwards sets the direction to right.
TEST_F(InputFixture, MoveRightSetsDirectionRight) {
  input_state().move_right = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  constexpr Vec2f expected_direction{1.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that moving up and right sets the normalised diagonal direction.
TEST_F(InputFixture, MoveUpAndRightSetsNormalisedDirection) {
  input_state().move_up = true;
  input_state().move_right = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  const Vec2f expected_direction{Vec2f{1.0F, -1.0F}.normalise()};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that moving down and left sets normalised diagonal direction.
TEST_F(InputFixture, MoveDownAndLeftSetsNormalisedDirection) {
  input_state().move_down = true;
  input_state().move_left = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  const Vec2f expected_direction{Vec2f{-1.0F, 1.0F}.normalise()};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}

/// Test that multiple game objects are handled independently.
TEST_F(InputFixture, MultipleGameObjectsHandledIndependently) {
  input_state().move_up = true;
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id_one);
  registry.add_component<components::Velocity>(game_object_id_one, 2.0F);
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id_two);
  registry.add_component<components::Velocity>(game_object_id_two, 3.0F);
  input_system(registry);
  constexpr Vec2f expected_direction{0.0F, -1.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id_one).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id_one).speed, 2.0F);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id_two).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id_two).speed, 3.0F);
}

/// Test that opposing inputs cancel out to a zero direction.
TEST_F(InputFixture, OpposingInputsCancelToZero) {
  input_state().move_up = true;
  input_state().move_down = true;
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::KeyboardControlled>(game_object_id);
  registry.add_component<components::Velocity>(game_object_id, 1.0F);
  input_system(registry);
  constexpr Vec2f expected_direction{0.0F, 0.0F};
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).direction, expected_direction);
  ASSERT_EQ(registry.get_component<components::Velocity>(game_object_id).speed, 1.0F);
}
}  // namespace exodus::ecs::systems
