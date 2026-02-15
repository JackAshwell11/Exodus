// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

namespace exodus {
/// A simple 2D camera class for managing the view into the game world.
class Camera {
 public:
  /// Construct the camera at the given position.
  explicit Camera(const Vec2f& position = {0.0F, 0.0F}) : position_(position) {}

  /// Get the position of the camera in world coordinates.
  ///
  /// @return The current position of the camera.
  [[nodiscard]] auto get_position() const -> Vec2f { return position_; }

  /// Set the position of the camera in world coordinates.
  ///
  /// @param position The new position of the camera.
  void set_position(const Vec2f& position) { position_ = position; }

  /// Move the camera by an offset in world coordinates.
  ///
  /// @param offset The vector to move the camera by.
  void move(const Vec2f& offset) { position_ += offset; }

  /// Get the zoom level of the camera.
  ///
  /// @return The current zoom level.
  [[nodiscard]] auto get_zoom() const -> float { return zoom_; }

  /// Set the zoom level of the camera.
  ///
  /// @param zoom The new zoom level.
  void set_zoom(const float zoom) { zoom_ = zoom; }

 private:
  /// The position of the camera in world coordinates.
  Vec2f position_;

  /// The zoom level of the camera.
  float zoom_{1.0F};
};
}  // namespace exodus
