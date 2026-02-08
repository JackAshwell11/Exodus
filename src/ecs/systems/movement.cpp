// Related header
#include "exodus/ecs/systems/movement.hpp"

// Local headers
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"

namespace exodus::ecs::systems {
void movement_system(Registry& registry, const float delta_time) {
  for (const auto& [velocity, transform] : registry.view<components::Velocity, components::Transform>()) {
    transform.position += velocity.direction * velocity.speed * delta_time;
  }
}
}  // namespace exodus::ecs::systems
