// Related header
#include "exodus/ecs/systems/movement_system.hpp"

// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"

namespace exodus::ecs::systems {
void MovementSystem::fixed_update(const double delta_time) const {
  for (const auto& [id, components] :
       registry_->get_game_object_components<components::Transform, components::Velocity>()) {
    auto& [transform, velocity] = components;
    transform->position += velocity->direction * velocity->speed * static_cast<float>(delta_time);
  }
}
}  // namespace exodus::ecs::systems
