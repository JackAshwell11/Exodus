// Ensure this file is only included once
#pragma once

// Std headers
#include <cstdint>

namespace exodus {
/// Represents unique identifiers for game objects
using GameObjectID = int;

/// Stores the different types of game objects available.
enum class GameObjectType : std::uint8_t {
  Player = 0U << 0U,  // 0
  Enemy = 1U << 0U,   // 1
};
}  // namespace exodus
