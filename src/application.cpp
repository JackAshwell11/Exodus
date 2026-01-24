// Related header
#include "exodus/application.hpp"

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

namespace exodus {
Application::Application(const std::string& title, const int width, const int height)
    : title_(title),
      width_(width),
      height_(height),
      window_(nullptr),
      gl_context_(nullptr),
      engine_(nullptr),
      running_(false) {}

Application::~Application() {
  engine_.reset();
  if (gl_context_ != nullptr) {
    SDL_GL_DeleteContext(gl_context_);
    gl_context_ = nullptr;
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
}

bool Application::initialise() {
  // Initialise SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Failed to initialise SDL: " << SDL_GetError() << "\n";
    return false;
  }

  // Set SDL OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetSwapInterval(1);

  // Create the SDL window
  window_ = SDL_CreateWindow(title_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_,
                             SDL_WINDOW_OPENGL);
  if (window_ == nullptr) {
    std::cerr << "Failed to create window: " << SDL_GetError() << "\n";
    SDL_Quit();
    return false;
  }

  // Create the OpenGL context
  gl_context_ = SDL_GL_CreateContext(window_);
  if (gl_context_ == nullptr) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window_);
    SDL_Quit();
    return false;
  }
  SDL_GL_MakeCurrent(window_, gl_context_);

  // Load OpenGL functions using GLAD
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == 0) {
    std::cerr << "Failed to load OpenGL functions\n";
    SDL_GL_DeleteContext(gl_context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
    return false;
  }

  // Create the engine
  engine_ = std::make_unique<Engine>();
  return true;
}

void Application::process_events() {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    switch (event.type) {
      case SDL_QUIT:
        running_ = false;
        break;
      default:
        break;
    }
  }
}

int Application::run() {
  // Initialise the application if not already initialised
  if (!initialise()) {
    return 1;
  }

  // Start the main loop
  running_ = true;
  uint64_t lastTime = SDL_GetTicks64();
  double accumulator = 0.0;

  // Loop until the application is no longer running
  while (running_) {
    // Calculate the delta time since the last frame
    const uint64_t currentTime = SDL_GetTicks64();
    const double deltaTime = static_cast<double>(currentTime - lastTime) * 0.001;
    lastTime = currentTime;

    // Handle SDL events
    process_events();

    // Update and render the engine
    accumulator += deltaTime;
    while (accumulator >= FIXED_TIMESTEP) {
      engine_->fixed_update(FIXED_TIMESTEP);
      accumulator -= FIXED_TIMESTEP;
    }
    engine_->update(deltaTime);
    engine_->render();
    SDL_GL_SwapWindow(window_);
  }
  return 0;
}
}  // namespace exodus
