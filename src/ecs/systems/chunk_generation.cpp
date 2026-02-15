// Related header
#include "exodus/ecs/systems/chunk_generation.hpp"

// Local headers
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/factories.hpp"
#include "exodus/generation/generator.hpp"

namespace exodus::ecs {
namespace {
/// The radius (in chunks) around the player within which chunks should be generated and loaded.
constexpr int CHUNK_GENERATION_RADIUS{2};

/// Generate a chunk of game objects at the specified chunk position and seed.
///
/// @param registry The registry to create the game objects in.
/// @param chunk_pos The position of the chunk to generate, in chunk coordinates.
/// @param seed The seed to use for generating the chunk.
void generate_chunk(Registry& registry, const Vec2i& chunk_pos, const int seed) {
  const auto chunk{generation::generate_chunk(chunk_pos, seed)};
  for (int i{0}; std::cmp_less(i, chunk.size()); i++) {
    create_game_object(registry, chunk.at(i),
                       {static_cast<float>(i % generation::CHUNK_SIZE) +
                            static_cast<float>(chunk_pos.x * static_cast<int>(generation::CHUNK_SIZE)),
                        static_cast<float>(i / generation::CHUNK_SIZE) +
                            static_cast<float>(chunk_pos.y * static_cast<int>(generation::CHUNK_SIZE))});
  }
}
}  // namespace

namespace systems {
auto get_generated_chunks() -> std::unordered_set<Vec2i>& {
  static std::unordered_set<Vec2i> generated_chunks;
  return generated_chunks;
}

void chunk_generation_system(Registry& registry) {
  for (const auto& [player, transform] : registry.view<components::Player, components::Transform>()) {
    const Vec2f player_position{transform.position};
    const Vec2i player_chunk_pos{static_cast<int>(std::floor(player_position.x / generation::CHUNK_SIZE)),
                                 static_cast<int>(std::floor(player_position.y / generation::CHUNK_SIZE))};
    for (int chunk_x{-CHUNK_GENERATION_RADIUS}; chunk_x <= CHUNK_GENERATION_RADIUS; chunk_x++) {
      for (int chunk_y{-CHUNK_GENERATION_RADIUS}; chunk_y <= CHUNK_GENERATION_RADIUS; chunk_y++) {
        const Vec2i chunk_pos{player_chunk_pos.x + chunk_x, player_chunk_pos.y + chunk_y};
        if (get_generated_chunks().contains(chunk_pos)) {
          continue;
        }
        generate_chunk(registry, chunk_pos, 0);
        get_generated_chunks().insert(chunk_pos);
      }
    }
  }
}
}  // namespace systems
}  // namespace exodus::ecs
