// Ensure this file is only included once
#pragma once

// Std headers
#include <memory>
#include <unordered_set>

// Local headers
#include "exodus/camera.hpp"
#include "exodus/rendering/renderer.hpp"

namespace exodus {
namespace ecs {
class Registry;
}  // namespace ecs

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
  /// @param delta_time The time interval since the last time the function was called.
  void update(float delta_time);

  /// Process fixed update logic for the game engine.
  ///
  /// @param delta_time The time interval since the last time the function was called.
  void fixed_update(float delta_time);

  /// Process rendering logic for the game engine.
  ///
  /// @param delta_time The time interval since the last time the function was called.
  /// @param renderer The renderer to use for drawing to the screen.
  void render(float delta_time, rendering::Renderer& renderer);

 private:
  /// Manages game objects and their components.
  std::unique_ptr<ecs::Registry> registry_;

  /// The camera used for rendering the game world.
  Camera camera_;

  /// The chunks which have already been generated.
  std::unordered_set<Vec2i> generated_chunks_;
};
}  // namespace exodus
