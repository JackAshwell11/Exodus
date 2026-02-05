// Ensure this file is only included once
#pragma once

// External headers
#include <glad/gl.h>

// Local headers
#include "exodus/ecs/system_base.hpp"

namespace exodus {
class Camera;

namespace ecs::systems {
/// Handles rendering of sprites.
class RenderSystem : public SystemBase {
 public:
  /// Construct the render system.
  explicit RenderSystem(Registry* registry);

  /// Destroy the render system.
  ~RenderSystem() override;

  /// Process rendering logic for the render system.
  ///
  /// @param camera The camera to render with.
  void render(const Camera& camera) const override;

 private:
  /// The OpenGL shader program used for rendering sprites.
  GLuint shader_program_{0};

  /// The OpenGL Vertex Array Object.
  GLuint vao_{0};

  /// The OpenGL Vertex Buffer Object.
  GLuint vbo_{0};

  /// The cached uniform location for the model matrix.
  GLint model_loc_{-1};

  /// The cached uniform location for the projection matrix.
  GLint projection_loc_{-1};
};
}  // namespace ecs::systems
}  // namespace exodus
