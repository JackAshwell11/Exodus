// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

namespace exodus::generation {
/// Generate an OpenSimplex noise value for the given coordinates.
///
/// This is an implementation of the OpenSimplex2S (Smooth) algorithm:
/// https://github.com/KdotJPG/OpenSimplex2/blob/master/java/OpenSimplex2S.java
///
/// @param seed The seed for the noise generation.
/// @param pos The 2D position to sample the noise at.
/// @return A float value representing the noise at the given coordinates.
auto noise(std::int64_t seed, Vec2d pos) -> float;
}  // namespace exodus::generation
