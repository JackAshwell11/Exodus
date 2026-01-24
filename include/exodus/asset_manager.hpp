// Ensure this file is only included once
#pragma once

// Std headers
#include <string>
#include <unordered_map>

// External headers
#include "glad/gl.h"

namespace exodus {
/// Stores information about an OpenGL texture.
struct TextureInfo {
  /// The OpenGL texture ID.
  GLuint id;

  /// The width of the texture.
  float width;

  /// The height of the texture.
  float height;
};

class AssetManager {
 public:
  /// Construct the asset manager.
  explicit AssetManager();

  /// Destroy the asset manager.
  ~AssetManager();

  /// Deleted copy constructor to prevent copying of asset resources.
  AssetManager(const AssetManager&) = delete;

  /// Deleted copy assignment operator to prevent copying of asset resources.
  auto operator=(const AssetManager&) -> AssetManager& = delete;

  /// Deleted move constructor to prevent moving of asset resources.
  AssetManager(AssetManager&&) = delete;

  /// Deleted move assignment operator to prevent moving of asset resources.
  auto operator=(AssetManager&&) -> AssetManager& = delete;

  /// Get or load an asset.
  ///
  /// @param path The path to the PNG/JPG file.
  /// @return The texture information including the OpenGL texture ID, width, and height.
  [[nodiscard]] auto get(const std::string& path) -> TextureInfo;

 private:
  /// The map of loaded assets.
  std::unordered_map<std::string, TextureInfo> assets_;
};
}  // namespace exodus
