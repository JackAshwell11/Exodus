// Ensure this file is only included once
#pragma once

// Std headers
#include <string>
#include <unordered_map>

// External headers
#include <glad/gl.h>

namespace exodus {
/// Manages game assets such as textures.
class AssetManager {
 public:
  /// Get the instance of the asset manager.
  static AssetManager& instance();

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
  /// @return The OpenGL texture ID.
  [[nodiscard]] auto get(std::string_view path) -> GLuint;

 private:
  /// Construct the asset manager.
  AssetManager();

  /// Destroy the asset manager.
  ~AssetManager();

  /// The map of loaded assets.
  std::unordered_map<std::string, GLuint> assets_;
};
}  // namespace exodus
