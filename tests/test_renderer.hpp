// Ensure this file is only included once
#pragma once

// Std headers
#include <array>

// Local headers
#include "exodus/rendering/renderer.hpp"

namespace exodus::rendering {
/// A simple test renderer that records published instances for inspection.
struct TestRenderer final : Renderer {
  /// Publish batches of renderable instances to the renderer.
  ///
  /// @param batches A map of texture IDs to their associated instances.
  /// @param projection_matrix A 4x4 projection matrix to apply to all instances.
  /// @param offset The offset which should be applied to all positions.
  void publish(RenderBatches batches, const std::array<float, PROJECTION_MATRIX_SIZE>& projection_matrix,
               const Vec2f& offset) override {
    render_batches = std::move(batches);
    proj_matrix = projection_matrix;
    render_offset = offset;
  }

  /// Flush all published batches to the screen.
  void flush() override { flush_called = true; }

  /// The batches of renderable instances published to the renderer.
  RenderBatches render_batches;

  /// The 4x4 projection matrix published to the renderer.
  std::array<float, PROJECTION_MATRIX_SIZE> proj_matrix{};

  /// The offset published to the renderer.
  Vec2f render_offset;

  /// Whether the flush method was called or not.
  bool flush_called{false};
};
}  // namespace exodus::rendering
