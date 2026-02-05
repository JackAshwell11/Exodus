// Related header
#include "exodus/factories.hpp"

// Local headers
#include "exodus/asset_manager.hpp"
#include "exodus/ecs/components/keyboard_controlled.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"

namespace exodus {
namespace {
/// The type of factory function to create a game object.
using FactoryFn = void (*)(Registry*, GameObjectID, const Vec2f&, std::string_view);

/// Describes how to construct a game object.
struct GameObjectDescription {
  /// The type of game object.
  GameObjectType game_object_type;

  /// The factory function to create the game object.
  FactoryFn factory;

  /// The path to the sprite asset.
  std::string_view sprite_path;
};

/// Add the components for the terrain game object.
const auto TERRAIN_FACTORY{[](Registry* registry, const GameObjectID game_object_id, const Vec2f& position,
                              const std::string_view sprite_path) -> void {
  const GLuint id = AssetManager::instance().get(sprite_path);
  registry->add_component<ecs::components::Transform>(game_object_id, position);
  registry->add_component<ecs::components::Sprite>(game_object_id, id, 0);
}};

/// Add the components for the player game object.
const auto PLAYER_FACTORY{[](Registry* registry, const GameObjectID game_object_id, const Vec2f& position,
                             const std::string_view sprite_path) -> void {
  GLuint tex = AssetManager::instance().get(sprite_path);
  registry->add_component<ecs::components::Transform>(game_object_id, position);
  registry->add_component<ecs::components::Velocity>(game_object_id, 200.0F);
  registry->add_component<ecs::components::KeyboardControlled>(game_object_id);
  registry->add_component<ecs::components::Sprite>(game_object_id, tex, 1, 2.0F);
}};

/// The table of game object descriptions for each tile type.
constexpr auto TILE_TABLE{std::to_array<GameObjectDescription>({
    /// Player
    {.game_object_type = GameObjectType::Player, .factory = PLAYER_FACTORY, .sprite_path = "/sprites/player.png"},

    /// Grass
    {.game_object_type = GameObjectType::Terrain,
     .factory = TERRAIN_FACTORY,
     .sprite_path = "/sprites/floor_grass.png"},

    /// Water
    {.game_object_type = GameObjectType::Terrain,
     .factory = TERRAIN_FACTORY,
     .sprite_path = "/sprites/floor_water.png"},

    /// Sand
    {.game_object_type = GameObjectType::Terrain, .factory = TERRAIN_FACTORY, .sprite_path = "/sprites/floor_sand.png"},

    /// Mountain
    {.game_object_type = GameObjectType::Terrain,
     .factory = TERRAIN_FACTORY,
     .sprite_path = "/sprites/floor_mountain.png"},
})};
}  // namespace

auto create_game_object(Registry* registry, const generation::TileType tile_type, const Vec2f& position)
    -> GameObjectID {
  // Check if the tile type exists
  const std::size_t index{static_cast<std::size_t>(tile_type)};
  if (index >= TILE_TABLE.size()) [[unlikely]] {
    return -1;
  }

  // Create the game object
  const auto& [game_object_type, factory, sprite_path]{TILE_TABLE.at(index)};
  const GameObjectID game_object_id{registry->create_game_object(game_object_type)};
  factory(registry, game_object_id, position, sprite_path);
  return game_object_id;
}
}  // namespace exodus
