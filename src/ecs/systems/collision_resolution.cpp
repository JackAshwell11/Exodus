// Related header
#include "exodus/ecs/systems/collision_resolution.hpp"

// Local headers
#include "exodus/ecs/components/collider.hpp"
#include "exodus/ecs/components/enemy.hpp"
#include "exodus/ecs/components/player.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"
#include "exodus/world_config.hpp"

namespace exodus::ecs::systems {
namespace {
/// A small epsilon to prevent division by zero when two entities fully overlap.
constexpr float EPSILON{0.0001F};

/// Controls how close entities are allowed to get to each other. Lower values result in tighter packing.
constexpr float CONTACT_DISTANCE_SCALE{0.5F};

/// The radius around the player where enemies are allowed to bunch up more.
constexpr float SWARM_RADIUS{WorldConfig::SPRITE_SIZE * 1.5F};

/// Controls how much separation is preserved when enemies are tightly packed around the player. Lower values allow
/// denser clustering near the player.
constexpr float SWARM_COMPRESSION_NEAR_PLAYER{0.2F};

/// The strength of the push that separates overlapping entities each frame.
constexpr float SEPARATION_STRENGTH{0.1F};

/// The maximum distance two entities can be pushed apart in a single frame.
constexpr float MAX_SEPARATION_STEP{WorldConfig::SPRITE_SIZE * 0.2F};

/// Stores physics data for a single game object.
struct Body {
  /// The transform component for the game object.
  components::Transform* transform;

  /// The collider for the game object.
  components::CircleCollider* collider;
};
}  // namespace

void collision_resolution_system(Registry& registry) {
  // Get the player's position so enemies can cluster near them
  Vec2f player_position{};
  auto player_view{registry.view<components::Player, components::Transform>()};
  if (const auto iter{player_view.begin()}; iter != player_view.end()) {
    player_position = std::get<1>(*iter).position;
  } else {
    return;
  }

  // Gather enemy bodies for collision resolution
  std::vector<Body> bodies;
  for (auto [_, transform, circle_collider] :
       registry.view<components::Enemy, components::Transform, components::CircleCollider>()) {
    bodies.push_back({.transform = &transform, .collider = &circle_collider});
  }

  // Iterate pairwise over all bodies and resolve collisions
  for (auto iter_one{bodies.begin()}; iter_one != bodies.end(); ++iter_one) {
    for (auto iter_two{std::next(iter_one)}; iter_two != bodies.end(); ++iter_two) {
      // Compute the distance between the two game objects
      const Vec2f distance{iter_two->transform->position - iter_one->transform->position};
      const float distance_squared{distance.length_squared()};

      // Check if the two game objects are far apart enough that we don't need to do collision resolution
      if (const float radius_sum{(iter_one->collider->radius + iter_two->collider->radius) * CONTACT_DISTANCE_SCALE};
          distance_squared >= radius_sum * radius_sum) {
        continue;
      }

      // Check if the pair are overlapping and skip collision resolution if they're not
      const float actual_distance{std::max(std::sqrt(distance_squared), EPSILON)};
      const float target{(iter_one->collider->radius + iter_two->collider->radius) * CONTACT_DISTANCE_SCALE};
      if (actual_distance >= target) {
        continue;
      }

      // Reduce separation strength when either body is close to the player (allows enemies to form a tighter swarm near
      // the target)
      const float body_one_distance_to_player{(iter_one->transform->position - player_position).length()};
      const float body_two_distance_to_player{(iter_two->transform->position - player_position).length()};
      const float proximity{
          std::clamp(std::max(body_one_distance_to_player, body_two_distance_to_player) / SWARM_RADIUS,
                     SWARM_COMPRESSION_NEAR_PLAYER, 1.0F)};

      // Determine the correction magnitude based on how much the two bodies overlap
      const float penetration{target - actual_distance};
      const float correction_mag{std::min(penetration * SEPARATION_STRENGTH * proximity, MAX_SEPARATION_STEP)};

      // Determine the correction force to apply to the bodies to separate them
      const Vec2f correction{(distance / actual_distance) * (correction_mag * 0.5F)};
      iter_one->transform->position -= correction;
      iter_two->transform->position += correction;
    }
  }
}
}  // namespace exodus::ecs::systems
