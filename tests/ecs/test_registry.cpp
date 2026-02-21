// Local headers
#include "exodus/ecs/registry.hpp"
#include "macros.hpp"

namespace exodus::ecs {
/// A test component with an integer value.
struct TestComponentOne {
  /// The integer value of the test component.
  int value{};
};

/// A test component with a float value.
struct TestComponentTwo {
  /// The float value of the test component.
  float value{};
};

/// Implements the fixture for the ecs/registry.hpp tests.
class RegistryFixture : public testing::Test {
 protected:
  /// The registry which manages game objects and components.
  Registry registry{};
};

/// Test that creating a new game object works correctly.
TEST_F(RegistryFixture, CreateGameObject) {
  const GameObjectID game_object_id{registry.create()};
  ASSERT_TRUE(registry.has(game_object_id));
}

/// Test that checking if a game object exists works correctly.
TEST_F(RegistryFixture, HasGameObject) {
  const GameObjectID game_object_id{registry.create()};
  ASSERT_TRUE(registry.has(game_object_id));
  ASSERT_FALSE(registry.has(game_object_id + 1));
}

/// Test that destroying an existing game object works correctly.
TEST_F(RegistryFixture, DestroyGameObject) {
  const GameObjectID game_object_id{registry.create()};
  ASSERT_TRUE(registry.has(game_object_id));
  registry.destroy(game_object_id);
  ASSERT_FALSE(registry.has(game_object_id));
}

/// Test that destroying a non-existent game object works correctly without crashing.
TEST_F(RegistryFixture, DestroyNonExistentGameObject) {
  ASSERT_FALSE(registry.has(999));
  registry.destroy(999);
  ASSERT_FALSE(registry.has(999));
}

/// Test that destroying the same game object twice does not crash.
TEST_F(RegistryFixture, DestroyTwice) {
  const GameObjectID game_object_id{registry.create()};
  registry.destroy(game_object_id);
  registry.destroy(game_object_id);  // Should not crash
  ASSERT_FALSE(registry.has(game_object_id));
}

/// Test that adding a component to a game object works correctly.
TEST_F(RegistryFixture, AddComponent) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 42);
  ASSERT_TRUE(registry.has_component<TestComponentOne>(game_object_id));
}

/// Test that retrieving a component from a game object works correctly.
TEST_F(RegistryFixture, GetComponent) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 42);
  const auto& [value]{registry.get_component<TestComponentOne>(game_object_id)};
  ASSERT_EQ(value, 42);
}

/// Test that checking if a game object has a specific component works correctly.
TEST_F(RegistryFixture, HasComponent) {
  const GameObjectID game_object_id{registry.create()};
  ASSERT_FALSE(registry.has_component<TestComponentOne>(game_object_id));
  registry.add_component<TestComponentOne>(game_object_id, 42);
  ASSERT_TRUE(registry.has_component<TestComponentOne>(game_object_id));
}

/// Test that checking for a component on a non-existent game object works correctly.
TEST_F(RegistryFixture, HasComponentOnNonExistentGameObject) {
  ASSERT_FALSE(registry.has_component<TestComponentOne>(0));
}

/// Test that adding the same component twice does not overwrite.
TEST_F(RegistryFixture, AddComponentTwice) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 1);
  registry.add_component<TestComponentOne>(game_object_id, 2);
  const auto& [value]{registry.get_component<TestComponentOne>(game_object_id)};
  ASSERT_EQ(value, 1);
}

/// Test that adding multiple different components to a game object works correctly.
TEST_F(RegistryFixture, AddMultipleComponents) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 1);
  registry.add_component<TestComponentTwo>(game_object_id, 2.0F);
  ASSERT_TRUE(registry.has_component<TestComponentOne>(game_object_id));
  ASSERT_TRUE(registry.has_component<TestComponentTwo>(game_object_id));
}

/// Test that retrieving multiple components from a game object works correctly.
TEST_F(RegistryFixture, GetMultipleComponents) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 1);
  registry.add_component<TestComponentTwo>(game_object_id, 2.0F);
  const auto& [value_one]{registry.get_component<TestComponentOne>(game_object_id)};
  const auto& [value_two]{registry.get_component<TestComponentTwo>(game_object_id)};
  ASSERT_EQ(value_one, 1);
  ASSERT_EQ(value_two, 2.0F);
}

/// Test that viewing game objects with two specific components works correctly.
TEST_F(RegistryFixture, ViewWithTwoComponents) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_one, 20.0F);
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  const auto count{std::ranges::count_if(registry.view<TestComponentOne, TestComponentTwo>(), [](const auto& pair) {
    const auto& [test_component_one, test_component_two]{pair};
    EXPECT_EQ(test_component_one.value, 10);
    EXPECT_EQ(test_component_two.value, 20.0F);
    return true;
  })};
  ASSERT_EQ(count, 1);
}

/// Test that viewing game objects with one specific component works correctly.
TEST_F(RegistryFixture, ViewWithOneComponent) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  const auto count{std::ranges::distance(registry.view<TestComponentOne>())};
  ASSERT_EQ(count, 2);
}

/// Test that viewing game objects with no matches for the components works correctly.
TEST_F(RegistryFixture, ViewWithNoMatches) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 10);
  const auto count{std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>())};
  ASSERT_EQ(count, 0);
}

/// Test that viewing game objects with two specific components returns all matches.
TEST_F(RegistryFixture, ViewWithTwoComponentsMultipleMatches) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_one, 20.0F);
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  registry.add_component<TestComponentTwo>(game_object_id_two, 40.0F);
  const auto count{std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>())};
  ASSERT_EQ(count, 2);
}

/// Test that destroying a game object removes its components.
TEST_F(RegistryFixture, DestroyRemovesComponents) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 1);
  ASSERT_TRUE(registry.has_component<TestComponentOne>(game_object_id));
  registry.destroy(game_object_id);
  ASSERT_FALSE(registry.has_component<TestComponentOne>(game_object_id));
}

/// Test that viewing components after destroying a game object works correctly.
TEST_F(RegistryFixture, ViewAfterDestroy) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_one, 20.0F);
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  registry.add_component<TestComponentTwo>(game_object_id_two, 40.0F);
  registry.destroy(game_object_id_one);
  const auto count{std::ranges::count_if(registry.view<TestComponentOne, TestComponentTwo>(), [](const auto& pair) {
    const auto& [comp1, comp2]{pair};
    EXPECT_EQ(comp1.value, 30);
    EXPECT_EQ(comp2.value, 40.0F);
    return true;
  })};
  ASSERT_EQ(count, 1);
}

/// Test that creating multiple game objects with unique IDs works correctly.
TEST_F(RegistryFixture, MultipleCreates) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  const GameObjectID game_object_id_three{registry.create()};
  ASSERT_TRUE(registry.has(game_object_id_one));
  ASSERT_TRUE(registry.has(game_object_id_two));
  ASSERT_TRUE(registry.has(game_object_id_three));
  ASSERT_NE(game_object_id_one, game_object_id_two);
  ASSERT_NE(game_object_id_two, game_object_id_three);
}

/// Test that the sparse array resizes correctly when adding components to game objects with large IDs.
TEST_F(RegistryFixture, SparseResize) {
  std::ranges::for_each(std::views::iota(0, 100), [&](const int) { registry.create(); });
  const GameObjectID large_game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(large_game_object_id, 100);
  ASSERT_TRUE(registry.has_component<TestComponentOne>(large_game_object_id));
  const auto& [value]{registry.get_component<TestComponentOne>(large_game_object_id)};
  ASSERT_EQ(value, 100);
}

/// Test that game object IDs are unique and not reused after destruction.
TEST_F(RegistryFixture, IdsAreUniqueAndNotReused) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.destroy(game_object_id_one);
  const GameObjectID game_object_id_two{registry.create()};
  const GameObjectID game_object_id_three{registry.create()};
  ASSERT_NE(game_object_id_one, game_object_id_two);
  ASSERT_NE(game_object_id_two, game_object_id_three);
  ASSERT_LT(game_object_id_one, game_object_id_two);
  ASSERT_LT(game_object_id_two, game_object_id_three);
}

/// Test that viewing components on an empty registry returns no results.
TEST_F(RegistryFixture, ViewOnEmptyRegistry) {
  const auto count{std::ranges::distance(registry.view<TestComponentOne>())};
  ASSERT_EQ(count, 0);
}

/// Test that destroying a game object removes all its components and prevents access.
TEST_F(RegistryFixture, DestroyRemovesAllComponents) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 1);
  registry.add_component<TestComponentTwo>(game_object_id, 2.0F);
  ASSERT_TRUE(registry.has_component<TestComponentOne>(game_object_id));
  ASSERT_TRUE(registry.has_component<TestComponentTwo>(game_object_id));
  registry.destroy(game_object_id);
  ASSERT_FALSE(registry.has_component<TestComponentOne>(game_object_id));
  ASSERT_FALSE(registry.has_component<TestComponentTwo>(game_object_id));
}

/// Test that Storage::remove() early returns when game_object_id >= sparse.size().
TEST_F(RegistryFixture, RemoveComponentBeyondSparseSize) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentTwo>(game_object_id_one, 1.0F);
  registry.add_component<TestComponentOne>(game_object_id_two, 42);
  registry.destroy(game_object_id_two);
  ASSERT_FALSE(registry.has(game_object_id_two));
}

/// Test that Storage::remove() early returns when index == INVALID_INDEX.
TEST_F(RegistryFixture, RemoveComponentWithInvalidIndex) {
  const GameObjectID game_object_id{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id, 42);
  registry.destroy(game_object_id);
  registry.destroy(game_object_id);
  ASSERT_FALSE(registry.has(game_object_id));
}

/// Test that views are cached and reused when accessed multiple times.
TEST_F(RegistryFixture, ViewCachingBasic) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentOne>(game_object_id_two, 20);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
}

/// Test that a view cache is invalidated when a relevant component is added.
TEST_F(RegistryFixture, ViewCacheInvalidatedOnComponentAdd) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_two, 20);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
}

/// Test that a view cache is invalidated when a game object is destroyed.
TEST_F(RegistryFixture, ViewCacheInvalidatedOnDestroy) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentOne>(game_object_id_two, 20);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
  registry.destroy(game_object_id_one);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
}

/// Test that view cache is invalidated when a new game object is created.
TEST_F(RegistryFixture, ViewCacheInvalidatedOnCreate) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  const GameObjectID game_object_id_two{registry.create()};
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  registry.add_component<TestComponentOne>(game_object_id_two, 20);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
}

/// Test that unrelated component changes don't invalidate unrelated views.
TEST_F(RegistryFixture, ViewCacheNotInvalidatedByUnrelatedComponents) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_two, 20.0F);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  const GameObjectID game_object_id_three{registry.create()};
  registry.add_component<TestComponentTwo>(game_object_id_three, 30.0F);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentTwo>()), 2);
}

/// Test that a multi-component view cache is invalidated when any relevant component is added.
TEST_F(RegistryFixture, MultiComponentViewCacheInvalidation) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_one, 20.0F);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 1);
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 1);
  registry.add_component<TestComponentTwo>(game_object_id_two, 40.0F);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 2);
}

/// Test that a view cache handles repeated adds of the same component type correctly.
TEST_F(RegistryFixture, ViewCacheWithRepeatedComponentAdds) {
  const GameObjectID game_object_id_one{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  registry.add_component<TestComponentOne>(game_object_id_one, 20);
  const auto count{std::ranges::count_if(registry.view<TestComponentOne>(), [](const auto& tuple) {
    const auto& [comp]{tuple};
    return comp.value == 10;
  })};
  ASSERT_EQ(count, 1);
}

/// Test that a view cache is properly invalidated when destroying objects with multiple components.
TEST_F(RegistryFixture, ViewCacheInvalidatedOnDestroyMultiComponent) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_one, 20.0F);
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  registry.add_component<TestComponentTwo>(game_object_id_two, 40.0F);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 2);
  registry.destroy(game_object_id_one);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 1);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentTwo>()), 1);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 1);
}

/// Test that multiple different view types maintain separate caches.
TEST_F(RegistryFixture, MultipleDifferentViewCaches) {
  const GameObjectID game_object_id_one{registry.create()};
  const GameObjectID game_object_id_two{registry.create()};
  registry.add_component<TestComponentOne>(game_object_id_one, 10);
  registry.add_component<TestComponentTwo>(game_object_id_one, 20.0F);
  registry.add_component<TestComponentOne>(game_object_id_two, 30);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentTwo>()), 1);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 1);
  registry.add_component<TestComponentTwo>(game_object_id_two, 40.0F);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne>()), 2);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentTwo>()), 2);
  ASSERT_EQ(std::ranges::distance(registry.view<TestComponentOne, TestComponentTwo>()), 2);
}
}  // namespace exodus::ecs
