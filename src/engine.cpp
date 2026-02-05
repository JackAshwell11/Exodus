// Related header
#include "exodus/engine.hpp"

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
}  // namespace

Engine::Engine() : registry_(std::make_unique<Registry>()) {
  registry_->add_system<ecs::systems::InputSystem>();
  registry_->add_system<ecs::systems::MovementSystem>();
  registry_->add_system<ecs::systems::RenderSystem>();
  create_game_object(registry_.get(), generation::TileType::Player, PLAYER_POSITION);
  const auto chunk{generation::generate_chunk({0, 0}, 0)};
  for (int i{0}; i < chunk.size(); i++) {
    create_game_object(
        registry_.get(), chunk.at(i),
        {static_cast<float>(i % generation::CHUNK_SIZE), static_cast<float>(i / generation::CHUNK_SIZE)});
  }
}

Engine::~Engine() = default;

void Engine::update(const double delta_time) {
  registry_->update(delta_time);

  // Focus on the player position
  const Vec2f player_position{
      registry_->get_component<ecs::components::Transform>(registry_->get_game_object_ids(GameObjectType::Player).at(0))
          ->position};
  camera_.move(player_position - camera_.get_position());
}

void Engine::fixed_update(const double delta_time) const { registry_->fixed_update(delta_time); }

void Engine::render() const { registry_->render(camera_); }
}  // namespace exodus
