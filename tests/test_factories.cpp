// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/factories.hpp"
#include "exodus/generation/generator.hpp"
#include "macros.hpp"

namespace exodus {
/// Implements the fixture for the factories.hpp tests.
class FactoriesFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and their components.
  ecs::Registry registry;

  /// Set up the fixture for the tests.
  void SetUp() override { get_tile_textures().clear(); }
};

/// Implements the parameterised fixture for create_game_object() tests.
class FactoriesParamFixture : public testing::TestWithParam<generation::TileType> {
 protected:
  /// The registry which manages game objects and their components.
  ecs::Registry registry;

  /// Set up the fixture for the tests.
  void SetUp() override { get_tile_textures().clear(); }
};

/// Test that get_tile_textures() returns a reference to the tile textures map.
TEST_F(FactoriesFixture, GetTileTexturesReturnsReference) {
  get_tile_textures()[generation::TileType::Player] = 1U;
  ASSERT_EQ(get_tile_textures()[generation::TileType::Player], 1U);
}

/// Test that create_game_object() does not create a game object for an invalid tile type.
TEST_F(FactoriesFixture, CreateGameObjectInvalidTileTypeDoesNothing) {
  // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
  create_game_object(registry, static_cast<generation::TileType>(10), {0.0F, 0.0F});
  ASSERT_EQ(std::ranges::distance(registry.view<ecs::components::Transform>()), 0);
}

/// Test that create_game_object() creates a game object for each valid tile type.
TEST_P(FactoriesParamFixture, CreateGameObjectCreatesGameObject) {
  create_game_object(registry, GetParam(), {10.0F, 20.0F});
  ASSERT_EQ(std::ranges::distance(registry.view<ecs::components::Transform>()), 1);
}

/// Create a parameterised test for each tile type.
INSTANTIATE_TEST_SUITE_P(FactoriesTests, FactoriesParamFixture,
                         testing::Values(generation::TileType::Player, generation::TileType::Grass,
                                         generation::TileType::Water, generation::TileType::Sand,
                                         generation::TileType::Mountain));
}  // namespace exodus
