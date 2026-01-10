// Stop SDL hijacking main()
#define SDL_MAIN_HANDLED

// Std headers
#include <array>
#include <iostream>

// External headers
#include <SDL.h>
#include <glad/gl.h>

namespace {
/// Holds data related to an OpenGL shader program and its associated buffers.
struct ShaderData {
  /// The shader's vertex array object ID.
  GLuint vao;

  /// The shader's vertex buffer object ID.
  GLuint vbo;

  /// The shader program ID.
  GLuint shader;
};

/// Compile a shader of the given type from the provided source code.
///
/// @param type - The type of shader to compile.
/// @param source - The source code of the shader as a string.
/// @return The compiled shader's OpenGL ID.
auto compileShader(const GLenum type, const std::string& source) -> GLuint {
  const GLuint shader = glCreateShader(type);
  const GLchar* src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (success == 0) {
    std::array<char, 512> buffer{};
    glGetShaderInfoLog(shader, static_cast<GLsizei>(buffer.size()), nullptr, buffer.data());
    std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader error:\n" << buffer.data() << "\n";
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

/// Create a triangle shader program.
///
/// @return A TriangleData struct containing the VAO, VBO, and shader program ID.
auto setupTriangle() -> ShaderData {
  // Define triangle vertex data
  constexpr std::array vertices = {
      0.0f,  0.5f,   // Vertex 1 (X, Y)
      -0.5f, -0.5f,  // Vertex 2 (X, Y)
      0.5f,  -0.5f   // Vertex 3 (X, Y)
  };

  // Setup the VAO and VBO
  ShaderData data{};
  glGenVertexArrays(1, &data.vao);
  glBindVertexArray(data.vao);
  glGenBuffers(1, &data.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, data.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

  // Define shaders
  const std::string vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;

    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
  )";

  const std::string fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    void main() {
        FragColor = vec4(0.2, 0.7, 0.9, 1.0);
    }
  )";

  // Create and link the shader program
  const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
  const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  data.shader = glCreateProgram();
  glAttachShader(data.shader, vertexShader);
  glAttachShader(data.shader, fragmentShader);
  glLinkProgram(data.shader);

  // Check for linking errors
  GLint success = 0;
  glGetProgramiv(data.shader, GL_LINK_STATUS, &success);
  if (success == 0) {
    std::array<char, 512> buffer{};
    glGetProgramInfoLog(data.shader, static_cast<GLsizei>(buffer.size()), nullptr, buffer.data());
    std::cerr << "Shader program linking error:\n" << buffer.data() << "\n";
  }

  // Clean up shaders as they are no longer needed after linking
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return data;
}

/// Renders a shader program using the provided VAO.
///
/// @param vao - The vertex array object to bind.
/// @param shader - The shader program to use for rendering.
/// @param vertexCount - The number of vertices to draw.
auto render(const GLuint vao, const GLuint shader, const GLsizei vertexCount) -> void {
  glUseProgram(shader);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
}  // namespace

/// The application entry point.
int main() {
  // Initialise SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Failed to initialise SDL: " << SDL_GetError() << "\n";
    return 1;
  }

  // Set SDL OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetSwapInterval(1);

  // Create the SDL window
  SDL_Window* window =
      SDL_CreateWindow("Exodus", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
  if (!window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  // Create the OpenGL context
  const SDL_GLContext glContext = SDL_GL_CreateContext(window);
  if (!glContext) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_GL_MakeCurrent(window, glContext);

  // Load OpenGL functions using GLAD
  if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress))) {
    std::cerr << "Failed to load OpenGL functions\n";
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";

  // Initialise triangle geometry and shaders
  const auto [vao, vbo, shaderProgram] = setupTriangle();

  // Start the SDL main loop
  bool running = true;
  while (running) {
    // Handle SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }

    // Set the viewport and clear the screen
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.05f, 0.08f, 0.1f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the triangle and swap the window buffers to display it
    render(vao, shaderProgram, 3);
    SDL_GL_SwapWindow(window);
  }

  // Clean up the program
  glDeleteProgram(shaderProgram);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
