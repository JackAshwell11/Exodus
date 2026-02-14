// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/math.hpp"

// Forward declare to avoid bringing in OpenGL headers
using GLuint = unsigned int;

namespace exodus::rendering {
/// Represents a single drawable instance.
struct RenderInstance {
  /// The position where the instance is drawn.
  Vec2f position;

  /// The scale of the instance.
  float scale;

  /// The depth value for sorting (lower values are drawn first).
  float depth;
};

/// A batch of renderable instances grouped by their associated texture ID.
using RenderBatches = std::unordered_map<GLuint, std::vector<RenderInstance>>;

/// A generic renderer interface which handles publishing batches of drawables and flushing them to the GPU.
class Renderer {
public:
  /// Destroy the renderer.
  virtual ~Renderer() = default;

  /// Publish batches of renderable instances to the renderer.
  ///
  /// @param batches A map of texture IDs to their associated instances.
  /// @param projection_matrix A 4x4 projection matrix to apply to all instances.
  /// @param offset The offset which should be applied to all positions.
  virtual void publish(RenderBatches batches, const std::array<float, 16>& projection_matrix, const Vec2f& offset) = 0;

  /// Flush all published batches to the screen.
  virtual void flush() = 0;
};
}  // namespace exodus::rendering
