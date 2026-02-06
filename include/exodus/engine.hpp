// Ensure this file is only included once
#pragma once

// Std headers
#include <memory>
#include <unordered_set>

// Local headers
#include "exodus/camera.hpp"

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
  void update(double delta_time);

  /// Process fixed update logic for the game engine.
  ///
  /// @param delta_time - The time interval since the last time the function was called.
  void fixed_update(double delta_time) const;

  /// Process rendering logic for the game engine.
  void render() const;

 private:
  /// Manages game objects, components, and systems that are registered.
  std::unique_ptr<Registry> registry_;

  /// The camera used for rendering the game world.
  Camera camera_;

  /// The chunks which have already been generated.
  std::unordered_set<Vec2i> generated_chunks_;
};
}  // namespace exodus
