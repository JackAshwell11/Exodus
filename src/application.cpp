// Related header
#include "exodus/application.hpp"

// Std headers
#include <iostream>
#include <ranges>

// External headers
#include <SDL.h>
#include <SDL_image.h>
#include <glad/gl.h>

// Local headers
#include "exodus/ecs/systems/input.hpp"
#include "exodus/engine.hpp"
#include "exodus/factories.hpp"
#include "rendering/opengl_renderer.hpp"

namespace {
/// The flags to initialise SDL_image with.
constexpr unsigned int IMG_INIT_FLAGS{static_cast<unsigned int>(IMG_INIT_PNG) |
                                      static_cast<unsigned int>(IMG_INIT_JPG)};

/// The sprite paths for each tile type, indexed by TileType enum value.
constexpr auto TILE_SPRITE_PATHS{std::to_array<std::string_view>({
    "/sprites/player.png",
    "/sprites/floor_grass.png",
    "/sprites/floor_water.png",
    "/sprites/floor_sand.png",
    "/sprites/floor_mountain.png",
})};

/// The fixed delta time for physics and other fixed-step updates (60 FPS).
constexpr float FIXED_TIMESTEP{1.0F / 60.0F};

/// Load an asset.
///
/// @param path The path to the PNG/JPG file.
/// @return The OpenGL texture ID.
auto load(const std::string_view path) -> GLuint {
  // Load the image
  const std::string sprite_path{EXODUS_ASSETS_DIR + std::string(path)};
  SDL_Surface* surface{IMG_Load(sprite_path.c_str())};
  if (surface == nullptr) {
    throw std::runtime_error("Failed to load texture '" + sprite_path + "': " + IMG_GetError());
  }

  // Determine if the image is RGB or RGBA
  GLenum format{GL_RGB};
  GLint internal_format{GL_RGB8};
  if (surface->format->BytesPerPixel == 4) {
    format = GL_RGBA;
    internal_format = GL_RGBA8;
  }

  // Generate an OpenGL texture
  GLuint texture_id{};
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

  // Set texture parameters and then unbind the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Free the surface and return the texture ID
  SDL_FreeSurface(surface);
  return texture_id;
}

/// Check if a key is currently held down.
///
/// @param keys The current keyboard state.
/// @param scancode The scancode of the key to check.
/// @return True if the key is held down, false otherwise.
[[nodiscard]] auto is_key_down(const std::span<const Uint8> keys, const SDL_Scancode scancode) -> bool {
  const auto idx{static_cast<size_t>(scancode)};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-avoid-unchecked-container-access)
  return idx < keys.size() && keys[idx] != 0;
}
}  // namespace

namespace exodus {
namespace {
/// Load the textures for each tile type into the asset manager.
void load_tile_textures() {
  std::unordered_map<generation::TileType, GLuint>& tile_textures{get_tile_textures()};
  for (size_t idx{0}; idx < TILE_SPRITE_PATHS.size(); idx++) {
    const auto tile_type{static_cast<generation::TileType>(idx)};
    if (tile_textures.contains(tile_type)) {
      continue;
    }
    tile_textures[tile_type] = load(TILE_SPRITE_PATHS.at(idx));
  }
}

/// Update the input state based on the current keyboard state.
void update_input_state() {
  // Get the currently pressed keys
  int key_count{0};
  const Uint8* raw{SDL_GetKeyboardState(&key_count)};
  if (raw == nullptr) {
    return;
  }

  // Determine what keys are pressed
  auto& [move_up, move_down, move_left, move_right]{input_state()};
  const std::span keys{raw, static_cast<size_t>(key_count)};
  move_up = is_key_down(keys, SDL_SCANCODE_W);
  move_down = is_key_down(keys, SDL_SCANCODE_S);
  move_left = is_key_down(keys, SDL_SCANCODE_A);
  move_right = is_key_down(keys, SDL_SCANCODE_D);
}
}  // namespace

Application::Application(std::string title, const Vec2f& size)
    : title_(std::move(title)),
      size_(size),
      window_(nullptr),
      gl_context_(nullptr),
      engine_(nullptr),
      running_(false) {}

Application::~Application() {
  for (const auto& texture_id : get_tile_textures() | std::views::values) {
    glDeleteTextures(1, &texture_id);
  }
  engine_.reset();
  if (gl_context_ != nullptr) {
    SDL_GL_DeleteContext(gl_context_);
    gl_context_ = nullptr;
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  IMG_Quit();
  SDL_Quit();
}

auto Application::initialise() -> bool {
  // Initialise SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Failed to initialise SDL: " << SDL_GetError() << "\n";
    return false;
  }

  // Initialise SDL_image
  if ((static_cast<unsigned int>(IMG_Init(IMG_INIT_FLAGS)) & IMG_INIT_FLAGS) != IMG_INIT_FLAGS) {
    std::cerr << "Failed to initialise SDL_image: " << IMG_GetError() << "\n";
    SDL_Quit();
    return false;
  }

  // Set SDL OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetSwapInterval(1);

  // Create the SDL window
  window_ = SDL_CreateWindow(title_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(size_.x),
                             static_cast<int>(size_.y), SDL_WINDOW_OPENGL);
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
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  if (gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress)) == 0) {
    std::cerr << "Failed to load OpenGL functions\n";
    SDL_GL_DeleteContext(gl_context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
    return false;
  }

  // Load the textures and create the engine
  load_tile_textures();
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

auto Application::run() -> int {
  // Initialise the application if not already initialised
  if (!initialise()) {
    return 1;
  }

  // Start the main loop
  running_ = true;
  uint64_t last_time{SDL_GetTicks64()};
  float accumulator{0.0F};

  // Loop until the application is no longer running
  rendering::OpenGLRenderer renderer;
  while (running_) {
    // Calculate the delta time since the last frame
    const uint64_t current_time{SDL_GetTicks64()};
    const float delta_time{static_cast<float>(current_time - last_time) * 0.001F};
    last_time = current_time;

    // Handle SDL events
    process_events();
    update_input_state();

    // Update and render the engine
    accumulator += delta_time;
    while (accumulator >= FIXED_TIMESTEP) {
      engine_->fixed_update(FIXED_TIMESTEP);
      accumulator -= FIXED_TIMESTEP;
    }
    engine_->update(delta_time);
    engine_->render(delta_time, renderer);
    SDL_GL_SwapWindow(window_);
  }
  return 0;
}
}  // namespace exodus
