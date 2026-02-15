// Ensure this file is only included once
#pragma once

// Std headers
#include <array>

// Local headers
#include "exodus/rendering/renderer.hpp"

// Forward declare to avoid bringing in OpenGL headers
using GLint = int;

namespace exodus::rendering {
/// An OpenGL implementation of the Renderer interface for rendering batches to the screen.
class OpenGLRenderer final : public Renderer {
 public:
  /// Construct the renderer.
  OpenGLRenderer();

  /// Destroy the renderer.
  ~OpenGLRenderer() override;

  /// Deleted copy constructor to prevent copying.
  OpenGLRenderer(const OpenGLRenderer&) = delete;

  /// Deleted copy assignment operator to prevent copying.
  auto operator=(const OpenGLRenderer&) -> OpenGLRenderer& = delete;

  /// Deleted move constructor to prevent moving.
  OpenGLRenderer(OpenGLRenderer&&) noexcept = delete;

  /// Deleted move assignment operator to prevent moving.
  auto operator=(OpenGLRenderer&&) noexcept -> OpenGLRenderer& = delete;

  /// Publish batches of renderable instances to the renderer.
  ///
  /// @param batches A map of texture IDs to their associated instances.
  /// @param projection_matrix A 4x4 projection matrix to apply to all instances.
  /// @param offset The offset which should be applied to all positions.
  void publish(RenderBatches batches, const std::array<float, PROJECTION_MATRIX_SIZE>& projection_matrix,
               const Vec2f& offset) override;

  /// Flush all published batches to the screen.
  void flush() override;

 private:
  /// The OpenGL shader program used for rendering sprites.
  GLuint shader_program_{0};

  /// The OpenGL Vertex Array Object.
  GLuint vao_{0};

  /// The OpenGL Vertex Buffer Object for quad vertices.
  GLuint quad_vbo_{0};

  /// The OpenGL Vertex Buffer Object for instance data.
  GLuint instance_vbo_{0};

  /// The cached uniform location for the projection matrix.
  GLint projection_loc_{-1};

  /// The cached uniform location for the camera offset.
  GLint camera_offset_loc_{-1};

  /// The batches of renderable instances to be rendered in the current frame.
  RenderBatches frame_batches_;

  /// The 4x4 projection matrix for the current frame.
  std::array<float, PROJECTION_MATRIX_SIZE> frame_projection_{};

  /// The offset to apply to all positions in the current frame.
  Vec2f frame_offset_;
};
}  // namespace exodus::rendering
