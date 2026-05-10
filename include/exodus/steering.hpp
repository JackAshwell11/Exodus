// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

namespace exodus {
/// The distance of the wander circle from the game object.
inline constexpr float WANDER_CIRCLE_DISTANCE{50.0F};

/// The radius of the wander circle.
inline constexpr float WANDER_CIRCLE_RADIUS{25.0F};

/// The default forward direction for the wander behaviour when the game object has no velocity.
inline constexpr Vec2f DEFAULT_WANDER_FORWARD{1, 0};

/// Allow a game object to move towards another game object.
///
/// @param current_position The position of the game object.
/// @param target_position The position of the target game object.
/// @return The calculated steering force for the seek behaviour.
constexpr auto seek(const Vec2f& current_position, const Vec2f& target_position) -> Vec2f {
  return (target_position - current_position).normalise();
}

/// Allow a game object to run away from another game object.
///
/// @param current_position The position of the game object.
/// @param target_position The position of the target game object.
/// @return The calculated steering force for the flee behaviour.
constexpr auto flee(const Vec2f& current_position, const Vec2f& target_position) -> Vec2f {
  return (current_position - target_position).normalise();
}

/// Moves a game object towards a target, slowing it down as it approaches to stop smoothly.
///
/// @param current_position The current position of the game object.
/// @param target_position The position of the target game object.
/// @param slowing_radius The distance from the target at which the game object should start slowing down.
/// @return The calculated steering force for the arrival behaviour.
constexpr auto arrive(const Vec2f& current_position, const Vec2f& target_position,
                      const float slowing_radius) -> Vec2f {
  const Vec2f direction{target_position - current_position};
  const float distance{direction.length()};
  if (distance == 0.0F) {
    return {0, 0};
  }
  return distance < slowing_radius ? direction.normalise() * (distance / slowing_radius) : direction.normalise();
}

/// Allow a game object to move towards another game object's predicted position.
///
/// @param current_position The position of the game object.
/// @param target_position The position of the target game object.
/// @param target_velocity The velocity of the target game object.
/// @return The calculated steering force for the pursuit behaviour.
constexpr auto pursue(const Vec2f& current_position, const Vec2f& target_position,
                      const Vec2f& target_velocity) -> Vec2f {
  return seek(current_position, target_position + target_velocity);
}

/// Allow a game object to run away from another game object's predicted position.
///
/// @param current_position The position of the game object.
/// @param target_position The position of the target game object.
/// @param target_velocity The velocity of the target game object.
/// @return The calculated steering force for the evasion behaviour.
constexpr auto evade(const Vec2f& current_position, const Vec2f& target_position,
                     const Vec2f& target_velocity) -> Vec2f {
  return flee(current_position, target_position + target_velocity);
}

/// Allow a game object to move in a random direction.
///
/// @param current_velocity The velocity of the game object.
/// @param displacement_angle The angle of the displacement force in radians.
/// @return The calculated steering force for the wander behaviour.
constexpr auto wander(const Vec2f& current_velocity, const float displacement_angle) -> Vec2f {
  const Vec2f forward{current_velocity.length_squared() > 0 ? current_velocity.normalise() : DEFAULT_WANDER_FORWARD};
  const Vec2f displacement{Vec2f{0, -WANDER_CIRCLE_RADIUS}.rotate(displacement_angle)};
  return (forward * WANDER_CIRCLE_DISTANCE + displacement).normalise();
}
}  // namespace exodus
