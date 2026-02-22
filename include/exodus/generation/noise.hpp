// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

namespace exodus::generation {
/// Generate an OpenSimplex noise value for the given coordinates.
///
/// OpenSimplex2S is a smooth noise function similar to Perlin noise, but designed
/// to reduce visible grid patterns and make terrain features look more natural
/// (see https://en.wikipedia.org/wiki/OpenSimplex_noise).
///
/// This is an implementation of the OpenSimplex2S (Smooth) algorithm:
/// https://github.com/KdotJPG/OpenSimplex2/blob/master/java/OpenSimplex2S.java
///
/// @param pos The 2D position to sample the noise at.
/// @param seed The seed for deterministic output.
/// @return A float value representing the noise at the given coordinates.
auto noise(const Vec2d& pos, std::int64_t seed) -> float;
}  // namespace exodus::generation
