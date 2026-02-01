// Related header
#include "exodus/engine.hpp"

// Local headers
#include "exodus/ecs/registry.hpp"
#include "exodus/ecs/systems/input_system.hpp"
#include "exodus/ecs/systems/movement_system.hpp"
#include "exodus/ecs/systems/render_system.hpp"
#include "exodus/factories.hpp"
#include "exodus/generation/generator.hpp"

namespace exodus {
Engine::Engine() : registry_(std::make_unique<Registry>()) {
  registry_->add_system<ecs::systems::InputSystem>();
  registry_->add_system<ecs::systems::MovementSystem>();
  registry_->add_system<ecs::systems::RenderSystem>();
  const auto chunk{generation::generate_chunk({0, 0}, 0)};
  for (int i{0}; i < chunk.size(); i++) {
    create_game_object(registry_.get(), chunk.at(i), {static_cast<float>(i % generation::CHUNK_SIZE), static_cast<float>(i / generation::CHUNK_SIZE)});
  }
}

Engine::~Engine() = default;

void Engine::update(const double delta_time) const { registry_->update(delta_time); }

void Engine::fixed_update(const double delta_time) const { registry_->fixed_update(delta_time); }

void Engine::render() const { registry_->render(); }
}  // namespace exodus
