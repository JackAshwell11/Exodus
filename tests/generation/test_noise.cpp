// Std headers
#include <random>
#include <ranges>
#include <vector>

// Local headers
#include "exodus/generation/noise.hpp"
#include "macros.hpp"

namespace {
/// The default seed for noise generation.
constexpr int64_t DEFAULT_SEED{12345L};
}  // namespace

namespace exodus::generation {
/// Test that identical inputs produce identical outputs.
TEST(OpenSimplex2STest, SameInputsProduceSameOutput) {
  for (const Vec2d& point : std::vector<Vec2d>{{1.234, 5.678}, {0.0, 0.0}, {-5.0, 5.0}}) {
    const float result_one{noise(DEFAULT_SEED, point)};
    const float result_two{noise(DEFAULT_SEED, point)};
    ASSERT_FLOAT_EQ(result_one, result_two);
  }
}

/// Test that different seeds produce different noise values.
TEST(OpenSimplex2STest, DifferentSeedsProduceDifferentResults) {
  const float result_one{noise(1L, {10.0, 20.0})};
  const float result_two{noise(2L, {10.0, 20.0})};
  const float result_three{noise(1000000L, {10.0, 20.0})};
  ASSERT_NE(result_one, result_two);
  ASSERT_NE(result_one, result_three);
  ASSERT_NE(result_two, result_three);
}

/// Test that extreme seed values (min, max, 0, -1) work correctly.
TEST(OpenSimplex2STest, ExtremeSeedValues) {
  ASSERT_NO_THROW(noise(std::numeric_limits<int64_t>::min(), {5.0, 5.0}));
  ASSERT_NO_THROW(noise(std::numeric_limits<int64_t>::max(), {5.0, 5.0}));
  ASSERT_NO_THROW(noise(0L, {5.0, 5.0}));
  ASSERT_NO_THROW(noise(-1L, {5.0, 5.0}));
}

/// Test that the origin (0,0) produces valid finite noise.
TEST(OpenSimplex2STest, OriginProducesValidNoise) {
  const float result{noise(DEFAULT_SEED, {0.0, 0.0})};
  ASSERT_TRUE(std::isfinite(result));
}

/// Test that integer coordinates produce valid finite noise values.
TEST(OpenSimplex2STest, IntegerCoordinates) {
  for (int x{-10}; x <= 10; x++) {
    for (int y{-10}; y <= 10; y++) {
      const float result{noise(DEFAULT_SEED, {static_cast<double>(x), static_cast<double>(y)})};
      ASSERT_TRUE(std::isfinite(result));
      ASSERT_FALSE(std::isnan(result));
    }
  }
}

/// Test that grid vertices and fractional positions produce valid noise.
TEST(OpenSimplex2STest, GridVertices) {
  const std::vector testValues{0.0, 1.0, -1.0, 10.0, -10.0, 0.5, -0.5};
  for (const double x : testValues) {
    for (const double y : testValues) {
      const float result{noise(DEFAULT_SEED, {x, y})};
      ASSERT_TRUE(std::isfinite(result));
      ASSERT_FALSE(std::isnan(result));
    }
  }
}

/// Test that outputs are finite across a wide range of coordinates.
TEST(OpenSimplex2STest, OutputIsFinite) {
  const std::vector testCoords{-1000.0, -100.0, -10.0, -1.0, 0.0, 1.0, 10.0, 100.0, 1000.0};
  for (const double x : testCoords) {
    for (const double y : testCoords) {
      const float result{noise(DEFAULT_SEED, {x, y})};
      ASSERT_TRUE(std::isfinite(result));
      ASSERT_FALSE(std::isnan(result));
    }
  }
}

/// Test that noise values stay within a reasonable range of [-1.0F, 1.0F].
TEST(OpenSimplex2STest, OutputRange) {
  float min{std::numeric_limits<float>::infinity()};
  float max{-std::numeric_limits<float>::infinity()};
  std::mt19937_64 generator{DEFAULT_SEED};
  std::uniform_real_distribution dist{-100.0, 100.0};
  for (int i{0}; i < 10000; i++) {
    const float result{noise(DEFAULT_SEED, {dist(generator), dist(generator)})};
    min = std::min(min, result);
    max = std::max(max, result);
  }
  ASSERT_GE(min, -1.0F);
  ASSERT_LE(max, 1.0F);
}

/// Test that nearby points have similar noise values (continuity).
TEST(OpenSimplex2STest, ContinuityNearbyPointsSimilar) {
  constexpr double delta{0.001};
  constexpr Vec2d center_point{5.0, 5.0};
  const float center{noise(DEFAULT_SEED, center_point)};
  const float right{noise(DEFAULT_SEED, center_point + Vec2d{delta, 0.0})};
  const float up{noise(DEFAULT_SEED, center_point + Vec2d{0.0, delta})};
  const float diagonal{noise(DEFAULT_SEED, center_point + Vec2d{delta, delta})};
  ASSERT_LT(std::abs(center - right), 0.1F);
  ASSERT_LT(std::abs(center - up), 0.1F);
  ASSERT_LT(std::abs(center - diagonal), 0.1F);
}

/// Test that noise changes gradually without sudden jumps (smoothness).
TEST(OpenSimplex2STest, SmoothnessGradualChanges) {
  constexpr Vec2d start_point{0.0, 0.0};
  float prev{noise(DEFAULT_SEED, start_point)};
  for (int i{1}; i <= 100; i++) {
    const float current{noise(DEFAULT_SEED, start_point + static_cast<double>(i) * 0.01)};
    ASSERT_LT(std::abs(current - prev), 0.5F);
    prev = current;
  }
}

/// Test that different quadrants produce different values (no artificial symmetry).
TEST(OpenSimplex2STest, QuadrantDifferences) {
  constexpr double offset{10.5};
  const float result_one{noise(DEFAULT_SEED, {offset, offset})};
  const float result_two{noise(DEFAULT_SEED, {-offset, offset})};
  const float result_three{noise(DEFAULT_SEED, {-offset, -offset})};
  const float result_four{noise(DEFAULT_SEED, {offset, -offset})};
  ASSERT_TRUE(std::isfinite(result_one) && std::isfinite(result_two) && std::isfinite(result_three) &&
              std::isfinite(result_four));
  ASSERT_TRUE(result_one != result_two || result_two != result_three || result_three != result_four ||
              result_four != result_one);
}

/// Test that swapping X and Y coordinates produces different results.
TEST(OpenSimplex2STest, XYSwapping) {
  const float result_one{noise(DEFAULT_SEED, {10.0, 20.0})};
  const float result_two{noise(DEFAULT_SEED, {20.0, 10.0})};
  ASSERT_NE(result_one, result_two);
}

/// Test that very small coordinates produce valid finite results.
TEST(OpenSimplex2STest, VerySmallCoordinates) {
  constexpr double small{std::numeric_limits<double>::epsilon()};
  const float result{noise(DEFAULT_SEED, {small, small})};
  ASSERT_TRUE(std::isfinite(result));
}

/// Test that very large coordinates produce valid finite results.
TEST(OpenSimplex2STest, VeryLargeCoordinates) {
  const float result{noise(DEFAULT_SEED, {1e6, 1e6})};
  ASSERT_TRUE(std::isfinite(result));
}

/// Test that negative coordinates produce valid finite results.
TEST(OpenSimplex2STest, NegativeCoordinates) {
  const float result_one{noise(DEFAULT_SEED, {-5.0, -5.0})};
  const float result_two{noise(DEFAULT_SEED, {-100.0, -200.0})};
  ASSERT_TRUE(std::isfinite(result_one));
  ASSERT_TRUE(std::isfinite(result_two));
}

/// Test that coordinates with mixed signs produce valid finite results.
TEST(OpenSimplex2STest, MixedSignCoordinates) {
  const float result_one{noise(DEFAULT_SEED, {-5.0, 5.0})};
  const float result_two{noise(DEFAULT_SEED, {5.0, -5.0})};
  ASSERT_TRUE(std::isfinite(result_one));
  ASSERT_TRUE(std::isfinite(result_two));
}

/// Test that noise values have a reasonable distribution (balanced positive/negative).
TEST(OpenSimplex2STest, ReasonableDistribution) {
  // Generate many noise samples
  std::mt19937_64 generator(DEFAULT_SEED);
  std::uniform_real_distribution dist(0.0, 100.0);
  std::vector<float> noise_results(10000);
  std::ranges::generate(noise_results, [&] { return noise(DEFAULT_SEED, {dist(generator), dist(generator)}); });
  const auto pos_count{std::ranges::count_if(noise_results, [](const float value) { return value > 0; })};
  const auto neg_count{std::ranges::count_if(noise_results, [](const float value) { return value < 0; })};

  // Distribution should be roughly balanced
  const float pos_ratio{static_cast<float>(pos_count) / 10000.0F};
  const float neg_ratio{static_cast<float>(neg_count) / 10000.0F};
  ASSERT_LT(std::abs(pos_ratio - neg_ratio), 0.1F);
  ASSERT_GT(pos_ratio, 0.1F);
  ASSERT_LT(pos_ratio, 0.9F);
  ASSERT_GT(neg_ratio, 0.1F);
  ASSERT_LT(neg_ratio, 0.9F);
}

/// Test that noise varies across different spatial locations.
TEST(OpenSimplex2STest, VariationAcrossSpace) {
  const float reference{noise(DEFAULT_SEED, {0.0, 0.0})};
  const auto same_count{std::ranges::count_if(std::views::iota(1, 100), [&](const int i) {
    return std::abs(noise(DEFAULT_SEED, {static_cast<double>(i) * 0.5F, static_cast<double>(i) * 0.3F}) - reference) <
           1e-6F;
  })};
  ASSERT_EQ(same_count, 0);
}

/// Test a known value at origin to detect unintended algorithm changes.
TEST(OpenSimplex2STest, KnownValueOrigin) {
  const float result{noise(DEFAULT_SEED, {0.0, 0.0})};
  ASSERT_TRUE(std::abs(result) < 1e-6F);
}

/// Test known values at unit square corners to detect algorithm changes.
TEST(OpenSimplex2STest, KnownValueUnitSquare) {
  const float result_one{noise(DEFAULT_SEED, {0.0, 0.0})};
  const float result_two{noise(DEFAULT_SEED, {1.0, 0.0})};
  const float result_three{noise(DEFAULT_SEED, {0.0, 1.0})};
  const float result_four{noise(DEFAULT_SEED, {1.0, 1.0})};
  ASSERT_TRUE(std::abs(result_one) < 1e-6F);
  ASSERT_FLOAT_EQ(result_two, -0.370477617F);
  ASSERT_FLOAT_EQ(result_three, 0.494716585F);
  ASSERT_FLOAT_EQ(result_four, -0.191894904F);
}

/// Test numerical precision with very small coordinate changes.
TEST(OpenSimplex2STest, NumericalPrecision) {
  constexpr double delta{std::numeric_limits<double>::epsilon()};
  constexpr Vec2d point{10.0, 20.0};
  const float result_one{noise(DEFAULT_SEED, point)};
  const float result_two{noise(DEFAULT_SEED, point + Vec2d{delta, 0.0})};
  const float result_three{noise(DEFAULT_SEED, point + Vec2d{0.0, delta})};
  ASSERT_LT(std::abs(result_one - result_two), 0.001F);
  ASSERT_LT(std::abs(result_one - result_three), 0.001F);
}

/// Test that fractional coordinates produce valid finite results.
TEST(OpenSimplex2STest, FractionalCoordinates) {
  const std::vector fractions{0.1, 0.25, 0.33, 0.5, 0.66, 0.75, 0.9, 0.99};
  for (const double x : fractions) {
    for (const double y : fractions) {
      const float result{noise(DEFAULT_SEED, {x, y})};
      ASSERT_TRUE(std::isfinite(result));
    }
  }
}

/// Test that noise varies significantly when sampled along the X axis.
TEST(OpenSimplex2STest, XAxisSampling) {
  std::vector<float> samples{};
  for (int i{0}; i < 20; i++) {
    samples.emplace_back(noise(DEFAULT_SEED, {static_cast<double>(i) * 0.5, 0.0}));
  }
  float min{samples.front()};
  float max{samples.front()};
  for (const float sample : samples) {
    min = std::min(min, sample);
    max = std::max(max, sample);
  }
  ASSERT_GT(max - min, 0.01F);
  ASSERT_LT(max - min, 2.0F);
}

/// Test that noise varies significantly when sampled along the Y axis.
TEST(OpenSimplex2STest, YAxisSampling) {
  std::vector<float> samples{};
  for (int i{0}; i < 20; i++) {
    samples.emplace_back(noise(DEFAULT_SEED, {0.0, static_cast<double>(i) * 0.5}));
  }
  float min{samples.front()};
  float max{samples.front()};
  for (const float sample : samples) {
    min = std::min(min, sample);
    max = std::max(max, sample);
  }
  ASSERT_GT(max - min, 0.01F);
  ASSERT_LT(max - min, 2.0F);
}

/// Test that noise varies significantly when sampled along a diagonal.
TEST(OpenSimplex2STest, DiagonalSampling) {
  std::vector<float> samples{};
  for (int i{0}; i < 20; i++) {
    samples.emplace_back(noise(DEFAULT_SEED, {static_cast<double>(i) * 0.5, static_cast<double>(i) * 0.5}));
  }
  float min{samples.front()};
  float max{samples.front()};
  for (const float sample : samples) {
    min = std::min(min, sample);
    max = std::max(max, sample);
  }
  ASSERT_GT(max - min, 0.01F);
  ASSERT_LT(max - min, 2.0F);
}
}  // namespace exodus::generation
