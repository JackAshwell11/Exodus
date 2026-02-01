// Related header
#include "exodus/asset_manager.hpp"

// Std headers
#include <ranges>
#include <stdexcept>

// External headers
#include <SDL_image.h>

namespace {
/// The flags to initialise SDL_image with.
constexpr unsigned int IMG_INIT_FLAGS =
    static_cast<unsigned int>(IMG_INIT_PNG) | static_cast<unsigned int>(IMG_INIT_JPG);
}  // namespace

namespace exodus {
AssetManager& AssetManager::instance() {
  static AssetManager instance;
  return instance;
}

AssetManager::AssetManager() {
  if ((static_cast<unsigned int>(IMG_Init(IMG_INIT_FLAGS)) & IMG_INIT_FLAGS) != IMG_INIT_FLAGS) {
    throw std::runtime_error("Failed to initialise SDL_image: " + std::string(IMG_GetError()));
  }
}

AssetManager::~AssetManager() {
  for (const GLuint& texture_id : assets_ | std::views::values) {
    glDeleteTextures(1, &texture_id);
  }
  IMG_Quit();
}

auto AssetManager::get(const std::string_view path) -> GLuint {
  // If it's already loaded, return the texture ID
  const std::string sprite_path{EXODUS_ASSETS_DIR + std::string(path)};
  if (assets_.contains(sprite_path)) {
    return assets_.at(sprite_path);
  }

  // Load the image
  SDL_Surface* surface = IMG_Load(sprite_path.c_str());
  if (surface == nullptr) {
    throw std::runtime_error("Failed to load texture '" + sprite_path + "': " + IMG_GetError());
  }

  // Determine if the image is RGB or RGBA
  GLenum format = GL_RGB;
  GLint internal_format = GL_RGB8;
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

  // Free the surface and store the texture ID
  SDL_FreeSurface(surface);
  assets_.emplace(sprite_path, texture_id);
  return texture_id;
}
}  // namespace exodus
