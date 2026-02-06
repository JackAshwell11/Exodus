// Related header
#include "exodus/engine.hpp"

// Std headers
#include <utility>

// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/input_system.hpp"
#include "exodus/ecs/systems/movement_system.hpp"
#include "exodus/ecs/systems/render_system.hpp"
#include "exodus/factories.hpp"
#include "exodus/generation/generator.hpp"

namespace exodus {
namespace {
/// The initial position of the player
constexpr Vec2f PLAYER_POSITION{generation::CHUNK_SIZE / 2.0F, generation::CHUNK_SIZE / 2.0F};

/// The radius (in chunks) around the player within which chunks should be generated and loaded.
constexpr int CHUNK_GENERATION_RADIUS{2};

/// Generate a chunk of game objects at the specified chunk position and seed.
///
/// @param registry The registry to create the game objects in.
/// @param chunk_pos The position of the chunk to generate, in chunk coordinates.
/// @param seed The seed to use for generating the chunk.
void generate_chunk(Registry* registry, const Vec2i chunk_pos, const int seed) {
  const auto chunk{generation::generate_chunk(chunk_pos, seed)};
  for (int i{0}; std::cmp_less(i, chunk.size()); i++) {
    create_game_object(registry, chunk.at(i), {static_cast<float>(i % generation::CHUNK_SIZE) + chunk_pos.x * generation::CHUNK_SIZE, static_cast<float>(i / generation::CHUNK_SIZE) + chunk_pos.y * generation::CHUNK_SIZE});
  }
}
}  // namespace

Engine::Engine() : registry_(std::make_unique<Registry>()) {
  registry_->add_system<ecs::systems::InputSystem>();
  registry_->add_system<ecs::systems::MovementSystem>();
  registry_->add_system<ecs::systems::RenderSystem>();
  create_game_object(registry_.get(), generation::TileType::Player, PLAYER_POSITION);
}

Engine::~Engine() = default;

void Engine::update(const double delta_time) {
  registry_->update(delta_time);

  // Generate any chunks that need to be generated
  const GameObjectID player_id{registry_->get_game_object_ids(GameObjectType::Player).at(0)};
  const Vec2f player_position{registry_->get_component<ecs::components::Transform>(player_id)->position};
  const Vec2i player_chunk_pos{static_cast<int>(std::floor(player_position.x / generation::CHUNK_SIZE)),
                               static_cast<int>(std::floor(player_position.y / generation::CHUNK_SIZE))};
  for (int chunk_x{-CHUNK_GENERATION_RADIUS}; chunk_x <= CHUNK_GENERATION_RADIUS; chunk_x++) {
    for (int chunk_y{-CHUNK_GENERATION_RADIUS}; chunk_y <= CHUNK_GENERATION_RADIUS; chunk_y++) {
      const Vec2i chunk_pos{player_chunk_pos.x + chunk_x, player_chunk_pos.y + chunk_y};
      if (generated_chunks_.contains(chunk_pos)) {
        continue;
      }
      generate_chunk(registry_.get(), chunk_pos, 0);
      generated_chunks_.insert(chunk_pos);
    }
  }

  // Focus on the player position
  camera_.move(player_position - camera_.get_position());
}

void Engine::fixed_update(const double delta_time) const { registry_->fixed_update(delta_time); }

void Engine::render() const { registry_->render(camera_); }
}  // namespace exodus
