// Related header
#include "exodus/factories.hpp"

// Std headers
#include <array>
#include <utility>

// Local headers
#include "exodus/ecs/components/collider.hpp"
#include "exodus/ecs/components/enemy.hpp"
#include "exodus/ecs/components/keyboard_controlled.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/generation/generator.hpp"
#include "exodus/world_config.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
namespace exodus {
namespace {
/// A sentinel value representing an uninitialised OpenGL texture ID (texture IDs start at 1).
constexpr GLuint INVALID_TEXTURE_ID{0};

/// The fraction of the player sprite size used for the collision radius. Higher values make the player more physically
/// solid and harder to pass through.
constexpr float PLAYER_COLLIDER_COVERAGE{0.9F};

/// The fraction of the enemy sprite size used for the collision radius. Higher values make enemies occupy more space
/// and reduces swarm density.
constexpr float ENEMY_COLLIDER_COVERAGE{0.25F};

/// The type of factory function to create a game object.
using FactoryFn = void (*)(ecs::Registry&, ecs::GameObjectID, const Vec2f&, GLuint);

/// Add the components for the player game object.
constexpr FactoryFn PLAYER_FACTORY{[](ecs::Registry& registry, const ecs::GameObjectID game_object_id,
                                      const Vec2f& position, const GLuint texture_id) -> void {
  registry.add_component<ecs::components::KeyboardControlled>(game_object_id);
  registry.add_component<ecs::components::Player>(game_object_id);
  registry.add_component<ecs::components::Sprite>(game_object_id, texture_id, 1, 2.0F);
  registry.add_component<ecs::components::Transform>(game_object_id, position);
  registry.add_component<ecs::components::Velocity>(game_object_id, 200.0F);
  registry.add_component<ecs::components::CircleCollider>(game_object_id,
                                                          WorldConfig::SPRITE_SIZE * 0.5F * PLAYER_COLLIDER_COVERAGE);
}};

/// Add the components for the enemy game object.
constexpr FactoryFn ENEMY_FACTORY{[](ecs::Registry& registry, const ecs::GameObjectID game_object_id,
                                     const Vec2f& position, const GLuint texture_id) -> void {
  registry.add_component<ecs::components::Enemy>(game_object_id);
  registry.add_component<ecs::components::Sprite>(game_object_id, texture_id, 1);
  registry.add_component<ecs::components::Transform>(game_object_id, position);
  registry.add_component<ecs::components::Velocity>(game_object_id, 20.0F);
  registry.add_component<ecs::components::CircleCollider>(game_object_id,
                                                          WorldConfig::SPRITE_SIZE * 0.5F * ENEMY_COLLIDER_COVERAGE);
}};

/// Add the components for the terrain game object.
constexpr FactoryFn TERRAIN_FACTORY{[](ecs::Registry& registry, const ecs::GameObjectID game_object_id,
                                       const Vec2f& position, const GLuint texture_id) -> void {
  registry.add_component<ecs::components::Sprite>(game_object_id, texture_id, 0);
  registry.add_component<ecs::components::Transform>(game_object_id, position);
}};

/// The table of game object descriptions for each tile type.
using TileFactoryEntry = std::pair<generation::TileType, FactoryFn>;
constexpr std::array TILE_TABLE{
    TileFactoryEntry{generation::TileType::Player, PLAYER_FACTORY},
    TileFactoryEntry{generation::TileType::Enemy, ENEMY_FACTORY},
    TileFactoryEntry{generation::TileType::Grass, TERRAIN_FACTORY},
    TileFactoryEntry{generation::TileType::Water, TERRAIN_FACTORY},
    TileFactoryEntry{generation::TileType::Hills, TERRAIN_FACTORY},
    TileFactoryEntry{generation::TileType::Mountain, TERRAIN_FACTORY},
};
}  // namespace

auto get_tile_textures() -> std::unordered_map<generation::TileType, GLuint>& {
  static std::unordered_map<generation::TileType, GLuint> tile_textures{};
  return tile_textures;
}

void create_game_object(ecs::Registry& registry, const generation::TileType& tile_type, const Vec2f& position) {
  // Check if the tile type exists
  FactoryFn factory{nullptr};
  for (const auto& [type, factoryFn] : TILE_TABLE) {
    if (type == tile_type) {
      factory = factoryFn;
    }
  }
  if (factory == nullptr) [[unlikely]] {
    return;
  }

  // Create the game object
  const ecs::GameObjectID game_object_id{registry.create()};
  const GLuint texture_id{
      get_tile_textures().try_emplace(tile_type, static_cast<GLuint>(INVALID_TEXTURE_ID)).first->second};
  factory(registry, game_object_id, position, texture_id);
}
}  // namespace exodus
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
