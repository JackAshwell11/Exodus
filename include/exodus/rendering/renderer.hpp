// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

// Forward declare to avoid bringing in OpenGL headers
using GLuint = unsigned int;

namespace exodus::rendering {
/// The number of elements in a 4x4 projection matrix.
inline constexpr size_t PROJECTION_MATRIX_SIZE{16};

/// Represents a single drawable instance.
struct RenderInstance {
  /// The position where the instance is drawn.
  Vec2f position;

  /// The scale of the instance.
  float scale{1.0F};

  /// The depth value for sorting (lower values are drawn first).
  float depth{0.0F};
};

/// A batch of renderable instances grouped by their associated texture ID.
using RenderBatches = std::unordered_map<GLuint, std::vector<RenderInstance>>;

/// A generic renderer interface which handles publishing batches of drawables and flushing them to the GPU.
class Renderer {
 public:
  /// Construct the renderer.
  Renderer() = default;

  /// Destroy the renderer.
  virtual ~Renderer() = default;

  /// Deleted copy constructor to prevent copying.
  Renderer(const Renderer&) = delete;

  /// Deleted copy assignment operator to prevent copying.
  auto operator=(const Renderer&) -> Renderer& = delete;

  /// Deleted move constructor to prevent moving.
  Renderer(Renderer&&) noexcept = delete;

  /// Deleted move assignment operator to prevent moving.
  auto operator=(Renderer&&) noexcept -> Renderer& = delete;

  /// Publish batches of renderable instances to the renderer.
  ///
  /// @param batches A map of texture IDs to their associated instances.
  /// @param projection_matrix A 4x4 projection matrix to apply to all instances.
  /// @param offset The offset which should be applied to all positions.
  virtual void publish(RenderBatches batches, const std::array<float, PROJECTION_MATRIX_SIZE>& projection_matrix,
                       const Vec2f& offset) = 0;

  /// Flush all published batches to the screen.
  virtual void flush() = 0;
};
}  // namespace exodus::rendering
