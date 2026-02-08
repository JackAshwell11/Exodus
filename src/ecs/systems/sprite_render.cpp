// Related header
#include "exodus/ecs/systems/sprite_render.hpp"

// Std headers
#include <array>
#include <string_view>

// External headers
#include <glad/gl.h>

// Local headers
#include "exodus/camera.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"

namespace {
/// The width of the screen.
constexpr float SCREEN_WIDTH{1280.0F};

/// The height of the screen.
constexpr float SCREEN_HEIGHT{720.0F};

/// The shader code for drawing the vertices of the sprites.
constexpr std::string_view VERTEX_SHADER_SOURCE{R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 uModel;
uniform mat4 uProjection;

void main() {
  gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
  TexCoord = aTexCoord;
}
)"};

/// The shader code for applying the texture to the sprites.
constexpr std::string_view FRAGMENT_SHADER_SOURCE{R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
  // TODO: Maybe remove this when sprites don't have black in RGB channels
  // Discard fragments with very low alpha (cutout transparency)
  vec4 texColor = texture(uTexture, TexCoord);
  if (texColor.a < 0.1) {
    discard;
  }
  FragColor = texColor;
}
)"};

/// The vertex data for a quad made of two triangles.
constexpr std::array QUAD_VERTICES{
    // First triangle
    0.0F, 1.0F, 0.0F, 1.0F,  // Top-left
    1.0F, 0.0F, 1.0F, 0.0F,  // Bottom-right
    0.0F, 0.0F, 0.0F, 0.0F,  // Bottom-left

    // Second triangle
    0.0F, 1.0F, 0.0F, 1.0F,  // Top-left
    1.0F, 1.0F, 1.0F, 1.0F,  // Top-right
    1.0F, 0.0F, 1.0F, 0.0F   // Bottom-right
};

/// The near-clipping plane for depth testing.
constexpr float NEAR_PLANE{-10.0F};

/// The far-clipping plane for depth testing.
constexpr float FAR_PLANE{10.0F};

/// The base size of a sprite texture.
constexpr int SPRITE_TEXTURE_SIZE{128};

/// The scale factor to apply to sprites.
constexpr float SPRITE_SCALE{0.25F};

/// The final size of a sprite in pixels after scaling.
constexpr float SPRITE_SIZE{SPRITE_TEXTURE_SIZE * SPRITE_SCALE};

/// Compile an OpenGL shader from source code.
///
/// @param type The type of shader to compile.
/// @param source The source code of the shader.
/// @return The OpenGL shader ID.
auto compile_shader(const GLenum type, const char* source) -> GLuint {
  const GLuint shader{glCreateShader(type)};
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  return shader;
}

/// Represents a global context for rendering, holding shared OpenGL resources and state.
class RenderContext {
 public:
  /// Get the singleton instance of the RenderContext.
  ///
  /// @return A reference to the RenderContext instance.
  static auto instance() -> RenderContext& {
    static RenderContext instance;
    return instance;
  }

  /// Deleted copy constructor to prevent copying of the singleton instance.
  RenderContext(const RenderContext&) = delete;

  /// Deleted copy assignment to prevent copying of the singleton instance.
  auto operator=(const RenderContext&) -> RenderContext& = delete;

  /// Deleted move constructor to prevent moving of the singleton instance.
  RenderContext(RenderContext&&) = delete;

  /// Deleted move assignment to prevent moving of the singleton instance.
  auto operator=(RenderContext&&) -> RenderContext& = delete;

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

 private:
  /// Construct the render context.
  RenderContext() {
    // Compile the sprite vertex and fragment shaders
    const GLuint vertex_shader{compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE.data())};
    const GLuint fragment_shader{compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE.data())};

    // Link the shaders into a shader program
    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vertex_shader);
    glAttachShader(shader_program_, fragment_shader);
    glLinkProgram(shader_program_);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Cache uniform locations for performance
    model_loc_ = glGetUniformLocation(shader_program_, "uModel");
    projection_loc_ = glGetUniformLocation(shader_program_, "uProjection");

    // Set up the quad VAO and VBO
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(float), QUAD_VERTICES.data(), GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Set up texture uniform
    glUseProgram(shader_program_);
    glUniform1i(glGetUniformLocation(shader_program_, "uTexture"), 0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
  }

  /// Destroy the render context.
  ~RenderContext() {
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteProgram(shader_program_);
  }
};
}  // namespace

namespace exodus::ecs::systems {
void sprite_render_system(Registry& registry, const Camera& camera) {
  // Clear the buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Bind the shader program and VAO
  const RenderContext& context{RenderContext::instance()};
  glUseProgram(context.shader_program_);
  glBindVertexArray(context.vao_);

  // Calculate the projection matrix
  const std::array projection_matrix{
      // Column 0 (scale X)
      2.0F / (SCREEN_WIDTH / camera.get_zoom()),
      0.0F,
      0.0F,
      0.0F,

      // Column 1 (scale Y)
      0.0F,
      -2.0F / (SCREEN_HEIGHT / camera.get_zoom()),
      0.0F,
      0.0F,

      // Column 2 (scale Z)
      0.0F,
      0.0F,
      -2.0F / (FAR_PLANE - NEAR_PLANE),
      0.0F,

      // Column 3 (translation)
      -1.0F,
      1.0F,
      -(FAR_PLANE + NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE),
      1.0F,
  };

  // Update the projection matrix
  glUniformMatrix4fv(context.projection_loc_, 1, GL_FALSE, projection_matrix.data());

  // Render all game objects with Transform and Sprite components
  for (auto const& [transform, sprite] : registry.view<components::Transform, components::Sprite>()) {
    // Compute the model matrix for the sprite
    const Vec2f camera_offset{(transform.position - camera.get_position()) * SPRITE_SIZE};
    const float scaled_size{SPRITE_SIZE * sprite.scale};
    const std::array model_matrix{
        // Column 0 (scale X)
        scaled_size,
        0.0F,
        0.0F,
        0.0F,

        // Column 1 (scale Y)
        0.0F,
        scaled_size,
        0.0F,
        0.0F,

        // Column 2 (Z)
        0.0F,
        0.0F,
        1.0F,
        0.0F,

        // Column 3 (translation)
        camera_offset.x + ((SCREEN_WIDTH - scaled_size) / 2.0F),
        camera_offset.y + ((SCREEN_HEIGHT - scaled_size) / 2.0F),
        static_cast<float>(sprite.depth),
        1.0F,
    };

    // Set the model matrix uniform and bind the sprite texture
    glUniformMatrix4fv(context.model_loc_, 1, GL_FALSE, model_matrix.data());
    glBindTexture(GL_TEXTURE_2D, sprite.texture_id);

    // Draw the sprite quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
}  // namespace exodus::ecs::systems
