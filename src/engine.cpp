// Related header
#include "exodus/engine.hpp"

// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/movement_system.hpp"

namespace exodus {
Engine::Engine() : registry_(std::make_unique<Registry>()) {
  registry_->add_system<ecs::systems::MovementSystem>();
  player_id_ = registry_->create_game_object(GameObjectType::Player);
  registry_->add_component<ecs::components::Transform, Vec2f>(player_id_, {0.0f, 0.0f});
  registry_->add_component<ecs::components::Velocity>(player_id_);
  registry_->get_component<ecs::components::Velocity>(player_id_)->direction = {1.0f, 0.0f};
}

Engine::~Engine() = default;

void Engine::update(const double delta_time) const { registry_->update(delta_time); }

void Engine::fixed_update(const double delta_time) const { registry_->fixed_update(delta_time); }

void Engine::render() const {}
}  // namespace exodus
