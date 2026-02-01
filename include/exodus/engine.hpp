// Ensure this file is only included once
#pragma once

// Std headers
#include <memory>

// Local headers
#include "exodus/game_object.hpp"

// Forward declarations
class Registry;

namespace exodus {
// Forward declarations
class AssetManager;

/// Represents the core game engine that manages the game state and updates.
class Engine {
 public:
  /// Construct the engine.
  explicit Engine();

  /// Destroy the engine.
  ~Engine();

  /// Deleted copy constructor to prevent copying.
  Engine(const Engine&) = delete;

  /// Deleted copy assignment operator to prevent copying.
  auto operator=(const Engine&) -> Engine& = delete;

  /// Deleted move constructor to prevent moving.
  Engine(Engine&&) = delete;

  /// Deleted move assignment operator to prevent moving.
  auto operator=(Engine&&) -> Engine& = delete;

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
}  // namespace exodus
