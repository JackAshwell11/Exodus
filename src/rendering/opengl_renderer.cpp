// Related header
#include "opengl_renderer.hpp"

// Std headers
#include <string_view>

// External headers
#include <glad/gl.h>

namespace {
/// The stride of quad vertex data (position and texture coordinates).
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

/// The number of vertices in a quad (2 triangles with 3 vertices each).
constexpr GLsizei QUAD_VERTEX_COUNT{static_cast<GLsizei>(QUAD_VERTICES.size()) / 4};

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

/// Create and link the shader program for sprite rendering.
///
/// @return The OpenGL shader program ID.
auto create_shader_program() -> GLuint {
  // Compile the sprite vertex and fragment shaders
  const GLuint vertex_shader{compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE.data())};
  const GLuint fragment_shader{compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE.data())};

  // Link the shaders into a shader program
  const GLuint program{glCreateProgram()};
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return program;
}
}  // namespace

namespace exodus::rendering {
OpenGLRenderer::OpenGLRenderer()
    : shader_program_{create_shader_program()},
      projection_loc_{glGetUniformLocation(shader_program_, "uProjection")},
      camera_offset_loc_{glGetUniformLocation(shader_program_, "uCameraOffset")} {
  // Initialise the VAO and VBOs for rendering quads and instance data
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glGenBuffers(1, &quad_vbo_);
  glGenBuffers(1, &instance_vbo_);

  // Configure the quad vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
  glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(float), QUAD_VERTICES.data(), GL_STATIC_DRAW);

  // Vertex position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, QUAD_VERTEX_STRIDE, nullptr);
  glEnableVertexAttribArray(0);

  // Texture coordinate attribute
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, QUAD_VERTEX_STRIDE, reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Configure the instance buffer
  glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_);

  // Instance offset attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, INSTANCE_STRIDE, nullptr);
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);

  // Instance scale attribute
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, INSTANCE_STRIDE, reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribDivisor(3, 1);

  // Instance depth attribute
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, INSTANCE_STRIDE, reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(4);
  glVertexAttribDivisor(4, 1);

  // Set up texture uniform
  glUseProgram(shader_program_);
  glUniform1i(glGetUniformLocation(shader_program_, "uTexture"), 0);

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
}

OpenGLRenderer::~OpenGLRenderer() {
  glDeleteBuffers(1, &instance_vbo_);
  glDeleteBuffers(1, &quad_vbo_);
  glDeleteVertexArrays(1, &vao_);
  glDeleteProgram(shader_program_);
}

void OpenGLRenderer::publish(RenderBatches batches, const std::array<float, PROJECTION_MATRIX_SIZE>& projection_matrix,
                             const Vec2f& offset) {
  frame_batches_ = std::move(batches);
  frame_projection_ = projection_matrix;
  frame_offset_ = offset;
}

void OpenGLRenderer::flush() {
  // Clear the buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Bind the shader program and VAO
  glUseProgram(shader_program_);
  glBindVertexArray(vao_);

  // Update the projection matrix and camera offset
  glUniformMatrix4fv(projection_loc_, 1, GL_FALSE, frame_projection_.data());
  glUniform2f(camera_offset_loc_, frame_offset_.x, frame_offset_.y);

  // Render each batch with instanced drawing
  for (const auto& [texture_id, instances] : frame_batches_) {
    // Upload instance data to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instances.size() * sizeof(RenderInstance)), instances.data(),
                 GL_DYNAMIC_DRAW);

    // Bind the texture and draw all instances
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArraysInstanced(GL_TRIANGLES, 0, QUAD_VERTEX_COUNT, static_cast<GLsizei>(instances.size()));
  }
  frame_batches_.clear();
}
}  // namespace exodus::rendering
