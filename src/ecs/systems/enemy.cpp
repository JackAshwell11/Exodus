// Related header
#include "exodus/ecs/systems/enemy.hpp"

// Local headers
#include "exodus/ecs/components/enemy.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/components/velocity.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/steering.hpp"

namespace exodus::ecs::systems {
void enemy_movement_system(Registry& registry) {
  // Get the player's position
  Vec2f player_position;
  auto view{registry.view<components::Player, components::Transform>()};
  if (const auto iter{view.begin()}; iter != view.end()) {
    const auto& [player, player_transform]{*iter};
    player_position = player_transform.position;
  } else {
    return;
  }

  // Steer the game object towards the player's position
  for (const auto& [enemy, transform, velocity] :
       registry.view<components::Enemy, components::Transform, components::Velocity>()) {
    velocity.direction = seek(transform.position, player_position);
  }
}
}  // namespace exodus::ecs::systems
