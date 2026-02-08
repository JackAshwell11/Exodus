// Related header
#include "exodus/ecs/systems/sprite_render.hpp"

// Std headers
#include <algorithm>
#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>

// External headers
#include <glad/gl.h>

// Local headers
#include <iostream>

#include "exodus/camera.hpp"
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"

namespace {
/// The base size of a sprite texture.
constexpr int SPRITE_TEXTURE_SIZE{128};

/// The width of the screen.
constexpr float SCREEN_WIDTH{1280.0F};

/// The height of the screen.
constexpr float SCREEN_HEIGHT{720.0F};

/// The near-clipping plane for depth testing.
constexpr float NEAR_PLANE{-10.0F};

/// The far-clipping plane for depth testing.
constexpr float FAR_PLANE{10.0F};

/// The scale factor to apply to sprites.
constexpr float SPRITE_SCALE{0.25F};

/// The final size of a sprite in pixels after scaling.
constexpr float SPRITE_SIZE{SPRITE_TEXTURE_SIZE * SPRITE_SCALE};

/// The stride of quad vertex data (position + texture coordinates).
constexpr size_t QUAD_VERTEX_STRIDE{4 * sizeof(float)};

/// The stride of instance data (offset + scale + depth).
constexpr size_t INSTANCE_STRIDE{4 * sizeof(float)};

/// The shader code for drawing the vertices of the sprites.
constexpr std::string_view VERTEX_SHADER_SOURCE{R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 aInstanceOffset;
layout (location = 3) in float aInstanceScale;
layout (location = 4) in float aInstanceDepth;

uniform mat4 uProjection;
uniform vec2 uCameraOffset;

out vec2 TexCoord;

void main() {
  vec2 worldPos = aPos * aInstanceScale + aInstanceOffset + uCameraOffset;
  gl_Position = uProjection * vec4(worldPos, aInstanceDepth, 1.0);
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
  GLuint shader_program{0};

  /// The OpenGL Vertex Array Object.
  GLuint vao{0};

  /// The OpenGL Vertex Buffer Object for quad vertices.
  GLuint quad_vbo{0};

  /// The OpenGL Vertex Buffer Object for instance data.
  GLuint instance_vbo{0};

  /// The cached uniform location for the projection matrix.
  GLint projection_loc{-1};

  /// The cached uniform location for the camera offset.
  GLint camera_offset_loc{-1};

 private:
  /// Construct the render context.
  RenderContext() {
    // Compile the sprite vertex and fragment shaders
    const GLuint vertex_shader{compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE.data())};
    const GLuint fragment_shader{compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE.data())};

    // Link the shaders into a shader program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Cache uniform locations for performance
    projection_loc = glGetUniformLocation(shader_program, "uProjection");
    camera_offset_loc = glGetUniformLocation(shader_program, "uCameraOffset");

    // Initialise the VAO and VBOs for rendering quads and instance data
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &quad_vbo);
    glGenBuffers(1, &instance_vbo);

    // Configure the quad vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(float), QUAD_VERTICES.data(), GL_STATIC_DRAW);

    // Vertex position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, QUAD_VERTEX_STRIDE, nullptr);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, QUAD_VERTEX_STRIDE, reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configure the instance buffer
    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo);

    // Instance offset attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, INSTANCE_STRIDE, nullptr);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Instance scale attribute
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, INSTANCE_STRIDE, reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Instance depth attribute
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, INSTANCE_STRIDE, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // Set up texture uniform
    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "uTexture"), 0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
  }

  /// Destroy the render context.
  ~RenderContext() {
    glDeleteBuffers(1, &instance_vbo);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader_program);
  }
};
}  // namespace

namespace exodus::ecs::systems {
void sprite_render_system(Registry& registry, const Camera& camera) {
  // Clear the buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Bind the shader program and VAO
  const RenderContext& context{RenderContext::instance()};
  glUseProgram(context.shader_program);
  glBindVertexArray(context.vao);

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

  // Update the projection matrix and camera offset
  glUniformMatrix4fv(context.projection_loc, 1, GL_FALSE, projection_matrix.data());
  const Vec2f camera_pixel_offset{camera.get_position() * -SPRITE_SIZE};
  glUniform2f(context.camera_offset_loc, camera_pixel_offset.x, camera_pixel_offset.y);

  // Batch sprites by texture ID
  std::unordered_map<GLuint, std::vector<float>> batches;
  for (const auto& [sprite, transform] : registry.view<components::Sprite, components::Transform>()) {
    // Get or create an instance batch and then add the sprite's instance data to it
    std::vector<float>& instance_data{batches[sprite.texture_id]};
    const Vec2f world_offset{transform.position * SPRITE_SIZE};
    const float scaled_sprite_size{sprite.scale * SPRITE_SIZE};
    const float center_x{(SCREEN_WIDTH - scaled_sprite_size) / 2.0F};
    const float center_y{(SCREEN_HEIGHT - scaled_sprite_size) / 2.0F};
    instance_data.push_back(world_offset.x + center_x);
    instance_data.push_back(world_offset.y + center_y);
    instance_data.push_back(scaled_sprite_size);
    instance_data.push_back(static_cast<float>(sprite.depth));
  }

  // Render each batch with instanced drawing
  for (const auto& [texture_id, instances] : batches) {
    // Upload instance data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, context.instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(float), instances.data(), GL_DYNAMIC_DRAW);

    // Bind the texture and draw all instances
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(instances.size() / 4));
  }
}
}  // namespace exodus::ecs::systems
