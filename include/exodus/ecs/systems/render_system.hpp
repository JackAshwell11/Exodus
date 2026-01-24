// Ensure this file is only included once
#pragma once

// External headers
#include <glad/gl.h>

// Local headers
#include "exodus/ecs/system_base.hpp"

namespace exodus::ecs::systems {
/// Handles rendering of sprites.
class RenderSystem : public SystemBase {
 public:
  /// Construct the render system.
  explicit RenderSystem(Registry* registry);

  /// Destroy the render system.
  ~RenderSystem() override;

  /// Process rendering logic for the render system.
  void render() const override;

 private:
  /// OpenGL shader program.
  GLuint shader_program_{0};

  /// OpenGL Vertex Array Object.
  GLuint vao_{0};

  /// OpenGL Vertex Buffer Object.
  GLuint vbo_{0};

  /// Cached uniform location for model matrix.
  GLint model_loc_{-1};
};
}  // namespace exodus::ecs::systems
