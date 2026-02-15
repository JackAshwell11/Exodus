// Local headers
#include "exodus/camera.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/camera_follow.hpp"
#include "macros.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/camera_follow.hpp tests.
class CameraFollowFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};

  /// The camera to be controlled by the system.
  Camera camera{{0.0F, 0.0F}};
};

/// Test that the camera does not move if there are no players.
TEST_F(CameraFollowFixture, NoPlayerDoesNotMoveCamera) {
  camera_follow_system(registry, camera);
  constexpr Vec2f expected_position{0.0F, 0.0F};
  ASSERT_EQ(camera.get_position(), expected_position);
}

/// Test that the camera moves to the player's position when there is one player.
TEST_F(CameraFollowFixture, SinglePlayerMovesCameraToPlayerPosition) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Player>(game_object_id);
  registry.add_component<components::Transform>(game_object_id, Vec2f{10.0F, 20.0F});
  camera_follow_system(registry, camera);
  constexpr Vec2f expected_position{10.0F, 20.0F};
  ASSERT_EQ(camera.get_position(), expected_position);
}

/// Test that the camera moves to the last player's position when there are multiple players.
TEST_F(CameraFollowFixture, MultiplePlayersMovesCameraToLastPlayerPosition) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<components::Player>(game_object_id_one);
  registry.add_component<components::Transform>(game_object_id_one, Vec2f{10.0F, 20.0F});
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<components::Player>(game_object_id_two);
  registry.add_component<components::Transform>(game_object_id_two, Vec2f{30.0F, 40.0F});
  camera_follow_system(registry, camera);
  constexpr Vec2f expected_position{30.0F, 40.0F};
  ASSERT_EQ(camera.get_position(), expected_position);
}

/// Test that game objects with only the Player component do not affect the camera.
TEST_F(CameraFollowFixture, GameObjectWithOnlyPlayerDoesNotMoveCamera) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Player>(game_object_id);
  camera_follow_system(registry, camera);
  constexpr Vec2f expected_position{0.0F, 0.0F};
  ASSERT_EQ(camera.get_position(), expected_position);
}

/// Test that game objects with only the Transform component do not affect the camera.
TEST_F(CameraFollowFixture, GameObjectWithOnlyTransformDoesNotMoveCamera) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{10.0F, 20.0F});
  camera_follow_system(registry, camera);
  constexpr Vec2f expected_position{0.0F, 0.0F};
  ASSERT_EQ(camera.get_position(), expected_position);
}

/// Test that the camera moves correctly from a non-zero initial position.
TEST_F(CameraFollowFixture, CameraMovesFromNonZeroPosition) {
  camera.set_position({5.0F, 5.0F});
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Player>(game_object_id);
  registry.add_component<components::Transform>(game_object_id, Vec2f{15.0F, 25.0F});
  camera_follow_system(registry, camera);
  constexpr Vec2f expected_position{15.0F, 25.0F};
  ASSERT_EQ(camera.get_position(), expected_position);
}
}  // namespace exodus::ecs::systems
