// Ensure this file is only included once
#pragma once

// Std headers
#include <memory>

// Local headers
#include "exodus/ecs/registry.hpp"

/// Represents the core game engine that manages the game state and updates.
class Engine {
 public:
  /// Initialise the object.
  explicit Engine();

  /// Process update logic for the game engine.
  ///
  /// @param delta_time - The time interval since the last time the function was called.
  void update(double delta_time) const;

  /// Process fixed update logic for the game engine.
  ///
  /// @param delta_time - The time interval since the last time the function was called.
  void fixed_update(double delta_time) const;

  /// Process rendering logic for the game engine.
  void render() const;

 private:
  /// Manages game objects, components, and systems that are registered.
  std::unique_ptr<Registry> registry_;
};
