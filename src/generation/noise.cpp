// Related header
#include "exodus/generation/noise.hpp"

// Std headers
#include <array>

namespace {
/// The normaliser for the gradient vectors.
constexpr float NORMALISER{0.05481866495625118F};

/// The squared radius for 2D simplex noise.
constexpr float RADIUS_SQUARED{2.0F / 3.0F};

/// The skewing factor for 2D simplex noise.
constexpr double SKEW_FACTOR{0.366025403784439};

/// The unskewing factor for 2D simplex noise.
constexpr float UNSKEW_FACTOR{-0.21132486540518713F};

/// The hash multiplier.
constexpr std::uint64_t HASH_MULTIPLIER{0x53A3F72DEEC546F5ULL};

/// The prime for the X coordinate.
constexpr std::uint64_t PRIME_X{0x5205402B9270C86FUL};

/// The prime for the Y coordinate.
constexpr std::uint64_t PRIME_Y{0x598CD327003817B5UL};

/// The number of bits for gradient hashing.
constexpr unsigned int GRADIENT_HASH_BITS{7U};

/// The hash table size for 2D gradients.
constexpr unsigned int GRADIENT_TABLE_SIZE{1U << GRADIENT_HASH_BITS};  // 128

/// The number of bits in the hash value.
constexpr unsigned int HASH_BITS{64U};

/// The hash-space step to move to the second simplex corner.
constexpr exodus::Vec2u64 CORNER_HASH_OFFSET{PRIME_X, PRIME_Y};

/// The offset for the double-step corner in the X direction.
constexpr exodus::Vec2f CORNER_OFFSET_DOUBLE_X{(3.0F * UNSKEW_FACTOR) + 2.0F, (3.0F * UNSKEW_FACTOR) + 1.0F};

/// The offset for the double-step corner in the Y direction.
constexpr exodus::Vec2f CORNER_OFFSET_DOUBLE_Y{(3.0F * UNSKEW_FACTOR) + 1.0F, (3.0F * UNSKEW_FACTOR) + 2.0F};

/// The base set of 2D gradient vectors.
constexpr std::array<exodus::Vec2f, 24> BASE_GRADIENTS{{
    {0.38268343236509F, 0.923879532511287F},    {0.923879532511287F, 0.38268343236509F},
    {0.923879532511287F, -0.38268343236509F},   {0.38268343236509F, -0.923879532511287F},
    {-0.38268343236509F, -0.923879532511287F},  {-0.923879532511287F, -0.38268343236509F},
    {-0.923879532511287F, 0.38268343236509F},   {-0.38268343236509F, 0.923879532511287F},
    {0.130526192220052F, 0.99144486137381F},    {0.608761429008721F, 0.793353340291235F},
    {0.793353340291235F, 0.608761429008721F},   {0.99144486137381F, 0.130526192220051F},
    {0.99144486137381F, -0.130526192220051F},   {0.793353340291235F, -0.60876142900872F},
    {0.608761429008721F, -0.793353340291235F},  {0.130526192220052F, -0.99144486137381F},
    {-0.130526192220052F, -0.99144486137381F},  {-0.608761429008721F, -0.793353340291235F},
    {-0.793353340291235F, -0.608761429008721F}, {-0.99144486137381F, -0.130526192220052F},
    {-0.99144486137381F, 0.130526192220051F},   {-0.793353340291235F, 0.608761429008721F},
    {-0.608761429008721F, 0.793353340291235F},  {-0.130526192220052F, 0.99144486137381F},
}};

/// Generate the pre-computed 2D gradient table.
///
/// @return The generated 2D gradient table.
constexpr auto generate_gradients() -> std::array<exodus::Vec2f, GRADIENT_TABLE_SIZE> {
  std::array<exodus::Vec2f, GRADIENT_TABLE_SIZE> gradients;
  constexpr float scale{1.0F / NORMALISER};
  for (std::size_t idx{0}; idx < gradients.size(); idx++) {
    gradients.at(idx) = BASE_GRADIENTS.at(idx % BASE_GRADIENTS.size()) * scale;
  }
  return gradients;
}

/// The pre-computed 2D gradient table.
constexpr std::array<exodus::Vec2f, GRADIENT_TABLE_SIZE> GRADIENTS{generate_gradients()};

/// Compute the XOR of a 64-bit integer and a 2D vector of 64-bit integers.
///
/// @param lhs The left-hand side 64-bit integer.
/// @param rhs The right-hand side 2D vector of 64-bit integers.
/// @return The result of the XOR operation.
constexpr auto operator^(const std::uint64_t lhs, const exodus::Vec2u64& rhs) -> std::uint64_t {
  return lhs ^ rhs.x ^ rhs.y;
}

/// Compute the gradient contribution for a simplex corner.
///
/// @param seed The noise seed.
/// @param base_hash The pre-hashed base coordinates.
/// @param offset The offset from the corner position.
/// @return The gradient contribution value.
constexpr auto gradient(const std::uint64_t seed, const exodus::Vec2u64& base_hash,
                        const exodus::Vec2f& offset) -> float {
  // Combine seed and cell hashes
  std::uint64_t hash{seed ^ base_hash};

  // Scramble with hash multiplier
  hash *= HASH_MULTIPLIER;
  hash ^= hash >> (HASH_BITS - GRADIENT_HASH_BITS + 1);

  // Return dot product of the gradient vector and the offset
  const unsigned int gradient_index{static_cast<unsigned int>(hash >> 1U) & (GRADIENT_TABLE_SIZE - 1U)};
  return GRADIENTS.at(gradient_index).dot(offset);
}
}  // namespace

namespace exodus::generation {
auto noise(const std::int64_t seed, const Vec2d pos) -> float {
  // Skew the input space to determine which simplex cell we're in
  const Vec2d simplex_pos{pos + (SKEW_FACTOR * pos.sum())};

  // Get base points and offsets
  const Vec2i base_point{static_cast<int>(std::floor(simplex_pos.x)), static_cast<int>(std::floor(simplex_pos.y))};
  const Vec2f simplex_offset{simplex_pos - base_point};

  // Compute the prime pre-multiplication for hashing the base simplex cell
  const Vec2u64 base_hash{Vec2u64{base_point} * CORNER_HASH_OFFSET};

  // Unskew offsets for contribution calculation
  const float unskew_displacement{simplex_offset.sum() * UNSKEW_FACTOR};
  const Vec2f unskewed_offset{simplex_offset + unskew_displacement};

  // A helper to add a corner contribution with standard falloff
  float value{0.0F};
  auto add_corner{[&](const Vec2f& offset, const Vec2u64& hash) -> void {
    if (const float attenuation{RADIUS_SQUARED - offset.length_squared()}; attenuation > 0.0F) {
      value += attenuation * attenuation * attenuation * attenuation * gradient(seed, hash, offset);
    }
  }};

  // Compute the noise value from the first corner (origin of simplex cell)
  add_corner(unskewed_offset, base_hash);

  // Compute the noise value from the second corner (opposite corner of simplex cell)
  constexpr float opposite_node_dist{1.0F + (2.0F * UNSKEW_FACTOR)};
  add_corner(unskewed_offset - opposite_node_dist, base_hash + CORNER_HASH_OFFSET);

  // Compute the noise value from the third and fourth corners (edge vertices)
  constexpr float single_step_offset{UNSKEW_FACTOR + 1.0F};
  const float x_y_diff{simplex_offset.x - simplex_offset.y};
  if (unskew_displacement < UNSKEW_FACTOR) {
    // Compute the first near region (X-dominant side)
    if (simplex_offset.x + x_y_diff > 1.0F) {
      add_corner(unskewed_offset - CORNER_OFFSET_DOUBLE_X, base_hash + Vec2u64{PRIME_X << 1U, PRIME_Y});
    } else {
      add_corner(unskewed_offset - Vec2f{UNSKEW_FACTOR, single_step_offset}, base_hash + Vec2u64{0, PRIME_Y});
    }

    // Compute the second near region (Y-dominant side)
    if (simplex_offset.y - x_y_diff > 1.0F) {
      add_corner(unskewed_offset - CORNER_OFFSET_DOUBLE_Y, base_hash + Vec2u64{PRIME_X, PRIME_Y << 1U});
    } else {
      add_corner(unskewed_offset - Vec2f{single_step_offset, UNSKEW_FACTOR}, base_hash + Vec2u64{PRIME_X, 0});
    }
  } else {
    // Compute the first far region (X-dominant side)
    if (simplex_offset.x + x_y_diff < 0.0F) {
      add_corner(unskewed_offset + Vec2f{single_step_offset, UNSKEW_FACTOR}, base_hash - Vec2u64{PRIME_X, 0});
    } else {
      add_corner(unskewed_offset - Vec2f{single_step_offset, UNSKEW_FACTOR}, base_hash + Vec2u64{PRIME_X, 0});
    }

    // Compute the second far region (Y-dominant side)
    if (simplex_offset.y < x_y_diff) {
      add_corner(unskewed_offset + Vec2f{UNSKEW_FACTOR, single_step_offset}, base_hash - Vec2u64{0, PRIME_Y});
    } else {
      add_corner(unskewed_offset - Vec2f{UNSKEW_FACTOR, single_step_offset}, base_hash + Vec2u64{0, PRIME_Y});
    }
  }
  return value;
}
}  // namespace exodus::generation
