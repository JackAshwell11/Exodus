// Std headers
#include <numbers>

// Local headers
#include "exodus/steering.hpp"
#include "macros.hpp"

namespace exodus {
namespace {
/// The tolerance for comparing floating-point values in the tests.
constexpr float EPSILON{1e-6F};

/// Asserts that two Vec2f objects are approximately equal within a specified tolerance.
///
/// @param actual The actual Vec2f value to compare.
/// @param expected The expected Vec2f value to compare against.
void assert_vec_near(const Vec2f& actual, const Vec2f& expected) {
  ASSERT_NEAR(actual.x, expected.x, EPSILON);
  ASSERT_NEAR(actual.y, expected.y, EPSILON);
}
}  // namespace


/// Test if a larger current position produces the correct seek force.
TEST(SteeringTest, SeekLargerCurrent) {
  constexpr Vec2f expected_seek_force{-0.70710677F, -0.70710677F};
  assert_vec_near(seek({100, 100}, {50, 50}), expected_seek_force);
}

/// Test if a larger target position produces the correct seek force.
TEST(SteeringTest, SeekLargerTarget) {
  constexpr Vec2f expected_seek_force{0.70710677F, 0.70710677F};
  assert_vec_near(seek({50, 50}, {100, 100}), expected_seek_force);
}

/// Test if two equal positions produce the correct seek force.
TEST(SteeringTest, SeekEqual) {
  assert_vec_near(seek({100, 100}, {100, 100}), Vec2f{});
}

/// Test if a negative current position produces the correct seek force.
TEST(SteeringTest, SeekNegativeCurrent) {
  constexpr Vec2f expected_seek_force{0.70710677F, 0.70710677F};
  assert_vec_near(seek({-50, -50}, {100, 100}), expected_seek_force);
}

/// Test if a negative target position produces the correct seek force.
TEST(SteeringTest, SeekNegativeTarget) {
  constexpr Vec2f expected_seek_force{-0.70710677F, -0.70710677F};
  assert_vec_near(seek({100, 100}, {-50, -50}), expected_seek_force);
}

/// Test if two negative positions produce the correct seek force.
TEST(SteeringTest, SeekNegativePositions) {
  assert_vec_near(seek({-50, -50}, {-50, -50}), Vec2f{});
}

/// Test if a larger current position produces the correct flee force.
TEST(SteeringTest, FleeLargerCurrent) {
  constexpr Vec2f expected_flee_force{0.70710677F, 0.70710677F};
  assert_vec_near(flee({100, 100}, {50, 50}), expected_flee_force);
}

/// Test if a larger target position produces the correct flee force.
TEST(SteeringTest, FleeLargerTarget) {
  constexpr Vec2f expected_flee_force{-0.70710677F, -0.70710677F};
  assert_vec_near(flee({50, 50}, {100, 100}), expected_flee_force);
}

/// Test if two equal positions produce the correct flee force.
TEST(SteeringTest, FleeEqual) {
  assert_vec_near(flee({100, 100}, {100, 100}), Vec2f{});
}

/// Test if a negative current position produces the correct flee force.
TEST(SteeringTest, FleeNegativeCurrent) {
  constexpr Vec2f expected_flee_force{-0.70710677F, -0.70710677F};
  assert_vec_near(flee({-50, -50}, {100, 100}), expected_flee_force);
}

/// Test if a negative target position produces the correct flee force.
TEST(SteeringTest, FleeNegativeTarget) {
  constexpr Vec2f expected_flee_force{0.70710677F, 0.70710677F};
  assert_vec_near(flee({100, 100}, {-50, -50}), expected_flee_force);
}

/// Test if two negative positions produce the correct flee force.
TEST(SteeringTest, FleeNegativePositions) {
  assert_vec_near(flee({-50, -50}, {-50, -50}), Vec2f{});
}

/// Test if a position outside the radius produces the correct arrive force.
TEST(SteeringTest, ArriveOutsideSlowingRange) {
  constexpr Vec2f expected_arrive_force{-0.70710677F, -0.70710677F};
  assert_vec_near(arrive({500, 500}, {}, 100), expected_arrive_force);
}

/// Test if a position on the radius produces the correct arrive force.
TEST(SteeringTest, ArriveOnSlowingRange) {
  constexpr Vec2f expected_arrive_force{-0.70710677F, -0.70710677F};
  assert_vec_near(arrive({135, 135}, {}, 100), expected_arrive_force);
}

/// Test if a position inside the radius produces the correct arrive force.
TEST(SteeringTest, ArriveInsideSlowingRange) {
  constexpr Vec2f expected_arrive_force{-0.70710677F, -0.70710677F};
  assert_vec_near(arrive({100, 100}, {}, 100), expected_arrive_force);
}

/// Test if a position near the target produces the correct arrive force.
TEST(SteeringTest, ArriveNearTarget) {
  constexpr Vec2f expected_arrive_force{-0.5F, -0.5F};
  assert_vec_near(arrive({50, 50}, {}, 100), expected_arrive_force);
}

/// Test if a position on the target produces the correct arrive force.
TEST(SteeringTest, ArriveOnTarget) {
  assert_vec_near(arrive({}, {}, 100), Vec2f{});
}

/// Test if a non-moving target produces the correct pursue force.
TEST(SteeringTest, PursueNonMovingTarget) {
  constexpr Vec2f expected_pursue_force{0.70710677F, 0.70710677F};
  assert_vec_near(pursue({}, {100, 100}, {}), expected_pursue_force);
}

/// Test if a moving target produces the correct pursue force.
TEST(SteeringTest, PursueMovingTarget) {
  constexpr Vec2f expected_pursue_force{0.4472136F, 0.89442719F};
  assert_vec_near(pursue({}, {100, 100}, {-50, 0}), expected_pursue_force);
}

/// Test if having the same position produces the correct pursue force.
TEST(SteeringTest, PursueSamePositions) {
  constexpr Vec2f expected_pursue_force_same_positions{-1.0F, 0.0F};
  assert_vec_near(pursue({}, {}, {-50, 0}), expected_pursue_force_same_positions);
}

/// Test if a non-moving target produces the correct evade force.
TEST(SteeringTest, EvadeNonMovingTarget) {
  constexpr Vec2f expected_evade_force{-0.70710677F, -0.70710677F};
  assert_vec_near(evade({}, {100, 100}, {}), expected_evade_force);
}

/// Test if a moving target produces the correct evade force.
TEST(SteeringTest, EvadeMovingTarget) {
  constexpr Vec2f expected_evade_force{-0.4472136F, -0.89442719F};
  assert_vec_near(evade({}, {100, 100}, {-50, 0}), expected_evade_force);
}

/// Test if having the same position produces the correct evade force.
TEST(SteeringTest, EvadeSamePositions) {
  constexpr Vec2f expected_evade_force_same_positions{1.0F, 0.0F};
  assert_vec_near(evade({}, {}, {-50, 0}), expected_evade_force_same_positions);
}

/// Test if a non-moving game object produces the correct wander force.
TEST(SteeringTest, WanderNonMoving) {
  constexpr Vec2f expected_wander_force{0.98512113F, -0.17186199F};
  assert_vec_near(wander({}, std::numbers::pi_v<float> / 3.0F), expected_wander_force);
}

/// Test if a moving game object produces the correct wander force.
TEST(SteeringTest, WanderMoving) {
  constexpr Vec2f expected_wander_force{0.76590121F, -0.64295828F};
  assert_vec_near(wander({100, -100}, std::numbers::pi_v<float> / 3.0F),
                expected_wander_force);
}

/// Test if a zero angle produces the correct wander force.
TEST(SteeringTest, WanderZeroAngle) {
  constexpr Vec2f expected_wander_force{0.8944272F, -0.4472136F};
  assert_vec_near(wander({}, 0.0F), expected_wander_force);
}
}  // namespace exodus
