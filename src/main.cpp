// Stop SDL hijacking main()
#define SDL_MAIN_HANDLED

// Std headers
#include <iostream>

// External headers
#include <SDL.h>
#include <glad/gl.h>

// Local headers
#include "exodus/engine.hpp"

namespace {
/// The fixed delta time for physics and other fixed-step updates (60 FPS).
constexpr double FIXED_TIMESTEP = 1.0 / 60.0;
}  // namespace

/// The application entry point.
auto main() -> int {
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
  if (window == nullptr) {
    std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  // Create the OpenGL context
  void* glContext = SDL_GL_CreateContext(window);
  if (glContext == nullptr) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  SDL_GL_MakeCurrent(window, glContext);

  // Load OpenGL functions using GLAD
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == 0) {
    std::cerr << "Failed to load OpenGL functions\n";
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Run the game loop
  exodus::Engine engine;
  bool running = true;
  uint64_t lastTime = SDL_GetTicks64();
  double accumulator = 0.0;
  while (running) {
    // Calculate the delta time since the last frame
    const uint64_t currentTime = SDL_GetTicks64();
    const double deltaTime = static_cast<double>(currentTime - lastTime) * 0.001;
    lastTime = currentTime;

    // Handle SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
        default:
          break;
      }
    }

    // Update and render the engine
    accumulator += deltaTime;
    while (accumulator >= FIXED_TIMESTEP) {
      engine.fixed_update(FIXED_TIMESTEP);
      accumulator -= FIXED_TIMESTEP;
    }
    engine.update(deltaTime);
    SDL_GL_SwapWindow(window);
  }

  // Clean up the program
  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
