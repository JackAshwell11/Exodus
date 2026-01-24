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
AssetManager::AssetManager() {
  if ((static_cast<unsigned int>(IMG_Init(IMG_INIT_FLAGS)) & IMG_INIT_FLAGS) != IMG_INIT_FLAGS) {
    throw std::runtime_error("Failed to initialise SDL_image: " + std::string(IMG_GetError()));
  }
}

AssetManager::~AssetManager() {
  for (const auto& texture_info : assets_ | std::views::values) {
    glDeleteTextures(1, &texture_info.id);
  }
  IMG_Quit();
}

auto AssetManager::get(const std::string& path) -> TextureInfo {
  // If it's already loaded, return the texture info
  if (assets_.contains(path)) {
    return assets_.at(path);
  }

  // Load the image
  SDL_Surface* surface = IMG_Load(path.c_str());
  if (surface == nullptr) {
    throw std::runtime_error("Failed to load texture '" + path + "': " + IMG_GetError());
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Store the texture info and free the surface
  TextureInfo texture_info{texture_id, static_cast<float>(surface->w), static_cast<float>(surface->h)};
  SDL_FreeSurface(surface);
  assets_.emplace(path, texture_info);
  return texture_info;
}
}  // namespace exodus
