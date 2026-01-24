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

  /// Manages loading and caching of assets.
  std::unique_ptr<AssetManager> asset_manager_;

  /// The game object ID of the player.
  GameObjectID player_id_;
};
}  // namespace exodus
