// Related header
#include "exodus/ecs/systems/render_system.hpp"

// Std headers
#include <array>
#include <string_view>

// Local headers
#include "exodus/ecs/components/sprite.hpp"
#include "exodus/ecs/components/transform.hpp"
#include "exodus/ecs/registry.hpp"

namespace {
/// The width of the screen.
constexpr float SCREEN_WIDTH = 1280.0f;

/// The height of the screen.
constexpr float SCREEN_HEIGHT = 720.0f;

/// The shader code for drawing the vertices of the sprites.
constexpr std::string_view VERTEX_SHADER_SOURCE = R"(
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
)";

/// The shader code for applying the texture to the sprites.
constexpr std::string_view FRAGMENT_SHADER_SOURCE = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
  FragColor = texture(uTexture, TexCoord);
}
)";

/// The vertex data for a quad made of two triangles.
constexpr std::array QUAD_VERTICES = {
    // First triangle
    0.0f, 1.0f, 0.0f, 1.0f,  // Top-left
    1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
    0.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left
    // Second triangle
    0.0f, 1.0f, 0.0f, 1.0f,  // Top-left
    1.0f, 1.0f, 1.0f, 1.0f,  // Top-right
    1.0f, 0.0f, 1.0f, 0.0f   // Bottom-right
};

/// The orthographic projection matrix for 2D rendering.
constexpr std::array ORTHO_PROJECTION = {2.0f / SCREEN_WIDTH,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         -2.0f / SCREEN_HEIGHT,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         0.0f,
                                         -1.0f,
                                         0.0f,
                                         -1.0f,
                                         1.0f,
                                         0.0f,
                                         1.0f};

/// Compile an OpenGL shader from source code.
///
/// @param type The type of shader to compile.
/// @param source The source code of the shader.
/// @return The OpenGL shader ID.
auto compile_shader(const GLenum type, const char* source) -> GLuint {
  const GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  return shader;
}
}  // namespace

namespace exodus::ecs::systems {
RenderSystem::RenderSystem(Registry* registry) : SystemBase(registry) {
  // Compile the sprite vertex and fragment shaders
  const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE.data());
  const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE.data());

  // Link the shaders into a shader program
  shader_program_ = glCreateProgram();
  glAttachShader(shader_program_, vertex_shader);
  glAttachShader(shader_program_, fragment_shader);
  glLinkProgram(shader_program_);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // Cache uniform locations for performance
  model_loc_ = glGetUniformLocation(shader_program_, "uModel");

  // Set up the quad VAO and VBO
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, QUAD_VERTICES.size() * sizeof(float), QUAD_VERTICES.data(), GL_STATIC_DRAW);

  // Define vertex attributes
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Set up the projection matrix and texture uniform
  glUseProgram(shader_program_);
  glUniformMatrix4fv(glGetUniformLocation(shader_program_, "uProjection"), 1, GL_FALSE, ORTHO_PROJECTION.data());
  glUniform1i(glGetUniformLocation(shader_program_, "uTexture"), 0);
}

RenderSystem::~RenderSystem() {
  glDeleteBuffers(1, &vbo_);
  glDeleteVertexArrays(1, &vao_);
  glDeleteProgram(shader_program_);
}

void RenderSystem::render() const {
  // Bind the shader program and VAO
  glUseProgram(shader_program_);
  glBindVertexArray(vao_);

  // Render all game objects with Transform and Sprite components
  for (const auto& [id, components] :
       registry_->get_game_object_components<components::Transform, components::Sprite>()) {
    const auto& [transform, sprite] = components;

    // Compute the model matrix for the sprite
    const std::array model_matrix = {
        sprite->width, 0.0f, 0.0f, 0.0f, 0.0f, sprite->height,        0.0f,
        0.0f,          0.0f, 0.0f, 1.0f, 0.0f, transform->position.x, transform->position.y,
        0.0f,          1.0f};

    // Set the model matrix uniform and bind the sprite texture
    glUniformMatrix4fv(model_loc_, 1, GL_FALSE, model_matrix.data());
    glBindTexture(GL_TEXTURE_2D, sprite->texture_id);

    // Draw the sprite quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}
}  // namespace exodus::ecs::systems
