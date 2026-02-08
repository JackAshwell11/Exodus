// Related header
#include "exodus/ecs/systems/camera_follow.hpp"

// Local headers
#include "exodus/camera.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"

namespace exodus::ecs::systems {
void camera_follow_system(Registry& registry, Camera& camera) {
  for (const auto& [player, transform] : registry.view<components::Player, components::Transform>()) {
    // Focus on the player position
    camera.move(transform.position - camera.get_position());
  }
}
}  // namespace exodus::ecs::systems
