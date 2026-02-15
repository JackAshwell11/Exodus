// Related header
#include "exodus/factories.hpp"

// Local headers
#include "exodus/ecs/components/keyboard_controlled.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
namespace exodus {
namespace {
/// A sentinel value representing an uninitialised OpenGL texture ID (texture IDs start at 1).
constexpr GLuint INVALID_TEXTURE_ID{0};

/// The type of factory function to create a game object.
using FactoryFn = void (*)(ecs::Registry&, ecs::GameObjectID, const Vec2f&, GLuint);

/// Add the components for the terrain game object.
constexpr FactoryFn TERRAIN_FACTORY{[](ecs::Registry& registry, const ecs::GameObjectID game_object_id,
                                       const Vec2f& position, const GLuint texture_id) -> void {
  registry.add_component<ecs::components::Sprite>(game_object_id, texture_id, 0);
  registry.add_component<ecs::components::Transform>(game_object_id, position);
}};

/// Add the components for the player game object.
constexpr FactoryFn PLAYER_FACTORY{[](ecs::Registry& registry, const ecs::GameObjectID game_object_id,
                                      const Vec2f& position, const GLuint texture_id) -> void {
  registry.add_component<ecs::components::KeyboardControlled>(game_object_id);
  registry.add_component<ecs::components::Player>(game_object_id);
  registry.add_component<ecs::components::Sprite>(game_object_id, texture_id, 1, 2.0F);
  registry.add_component<ecs::components::Transform>(game_object_id, position);
  registry.add_component<ecs::components::Velocity>(game_object_id, 200.0F);
}};

/// The table of game object descriptions for each tile type.
constexpr auto TILE_TABLE{std::to_array<FactoryFn>({
    /// Player
    PLAYER_FACTORY,

    /// Grass
    TERRAIN_FACTORY,

    /// Water
    TERRAIN_FACTORY,

    /// Sand
    TERRAIN_FACTORY,

    /// Mountain
    TERRAIN_FACTORY,
})};
}  // namespace

auto get_tile_textures() -> std::unordered_map<generation::TileType, GLuint>& {
  static std::unordered_map<generation::TileType, GLuint> tile_textures{};
  return tile_textures;
}

void create_game_object(ecs::Registry& registry, const generation::TileType& tile_type, const Vec2f& position) {
  // Check if the tile type exists
  const std::size_t index{static_cast<std::size_t>(tile_type)};
  if (index >= TILE_TABLE.size()) [[unlikely]] {
    return;
  }

  // Create the game object
  const auto& factory{TILE_TABLE.at(index)};
  const ecs::GameObjectID game_object_id{registry.create()};
  const GLuint texture_id{
      get_tile_textures().try_emplace(tile_type, static_cast<GLuint>(INVALID_TEXTURE_ID)).first->second};
  factory(registry, game_object_id, position, texture_id);
}
}  // namespace exodus
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
