// Local headers
#include "exodus/camera.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/sprite_render.hpp"
#include "macros.hpp"
#include "test_renderer.hpp"

namespace exodus::ecs::systems {
/// Implements the fixture for the ecs/systems/sprite_render.hpp tests.
class SpriteRenderFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  Registry registry{};

  /// The camera used for rendering.
  Camera camera{{0.0F, 0.0F}};

  /// The test renderer to capture output.
  rendering::TestRenderer renderer{};
};

/// Test that the sprite render system does not publish anything if there are no sprites.
TEST_F(SpriteRenderFixture, NoSpritesDoesNotPublishAnything) {
  sprite_render_system(registry, camera, renderer);
  ASSERT_TRUE(renderer.render_batches.empty());
}

/// Test that game objects without a Transform component are ignored.
TEST_F(SpriteRenderFixture, IgnoresGameObjectsWithoutTransform) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 1.0F);
  sprite_render_system(registry, camera, renderer);
  ASSERT_TRUE(renderer.render_batches.empty());
}

/// Test that game objects without a Sprite component are ignored.
TEST_F(SpriteRenderFixture, IgnoresGameObjectsWithoutSprite) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  ASSERT_TRUE(renderer.render_batches.empty());
}

/// Test that a single sprite is published correctly.
TEST_F(SpriteRenderFixture, SingleSpritePublishesCorrectInstance) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  ASSERT_EQ(renderer.render_batches.size(), 1U);
  ASSERT_EQ(renderer.render_batches.at(1U).size(), 1U);
  const auto& [position, scale, depth]{renderer.render_batches.at(1U).at(0)};
  constexpr Vec2f expected_position{624.0F, 344.0F};
  ASSERT_EQ(position, expected_position);
  ASSERT_EQ(scale, 32.0F);
  ASSERT_EQ(depth, 0.0F);
}

/// Test that a sprite with a custom scale is published correctly.
TEST_F(SpriteRenderFixture, SpriteWithCustomScale) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 2.0F);
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  ASSERT_EQ(renderer.render_batches.size(), 1U);
  ASSERT_EQ(renderer.render_batches.at(1U).size(), 1U);
  const auto& [position, scale, depth]{renderer.render_batches.at(1U).at(0)};
  constexpr Vec2f expected_position{608.0F, 328.0F};
  ASSERT_EQ(position, expected_position);
  ASSERT_EQ(scale, 64.0F);
  ASSERT_EQ(depth, 0.0F);
}

/// Test that a sprite at a non-zero position is published correctly.
TEST_F(SpriteRenderFixture, SpriteAtNonZeroPosition) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id, Vec2f{1.0F, 1.0F});
  sprite_render_system(registry, camera, renderer);
  constexpr Vec2f expected_position{656.0F, 376.0F};
  ASSERT_EQ(renderer.render_batches.at(1U).at(0).position, expected_position);
}

/// Test that multiple sprites with the same texture are batched together.
TEST_F(SpriteRenderFixture, MultipleSpritesSameTexture) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<components::Sprite>(game_object_id_one, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id_one, Vec2f{0.0F, 0.0F});
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<components::Sprite>(game_object_id_two, 1U, 1, 1.0F);
  registry.add_component<components::Transform>(game_object_id_two, Vec2f{1.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  ASSERT_EQ(renderer.render_batches.size(), 1U);
  ASSERT_EQ(renderer.render_batches.at(1U).size(), 2U);
  ASSERT_EQ(renderer.render_batches.at(1U).at(0).depth, 0.0F);
  ASSERT_EQ(renderer.render_batches.at(1U).at(1).depth, 1.0F);
}

/// Test that sprites with different textures are in separate batches.
TEST_F(SpriteRenderFixture, SpritesWithDifferentTextures) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<components::Sprite>(game_object_id_one, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id_one, Vec2f{0.0F, 0.0F});
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<components::Sprite>(game_object_id_two, 2U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id_two, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  ASSERT_EQ(renderer.render_batches.size(), 2U);
  ASSERT_EQ(renderer.render_batches.at(1U).size(), 1U);
  ASSERT_EQ(renderer.render_batches.at(2U).size(), 1U);
}

/// Test that the projection matrix is set correctly.
TEST_F(SpriteRenderFixture, ProjectionMatrix) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  constexpr std::array expected_projection_matrix{
      0.0015625F, 0.0F, 0.0F, 0.0F, 0.0F, -0.00277777785F, 0.0F, 0.0F, 0.0F, 0.0F, -0.1F, 0.0F, -1.0F, 1.0F, 0.0F, 1.0F,
  };
  ASSERT_EQ(renderer.proj_matrix, expected_projection_matrix);
}

/// Test that the camera offset is calculated correctly.
TEST_F(SpriteRenderFixture, CameraOffset) {
  camera.set_position({10.0F, 20.0F});
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  constexpr Vec2f expected_offset{-320.0F, -640.0F};
  ASSERT_EQ(renderer.render_offset, expected_offset);
}

/// Checks that flush() is not called as the caller should call it after all systems have run.
TEST_F(SpriteRenderFixture, FlushNotCalled) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<components::Sprite>(game_object_id, 1U, 0, 1.0F);
  registry.add_component<components::Transform>(game_object_id, Vec2f{0.0F, 0.0F});
  sprite_render_system(registry, camera, renderer);
  ASSERT_FALSE(renderer.flush_called);
}
}  // namespace exodus::ecs::systems
