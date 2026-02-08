// Ensure this file is only included once
#pragma once

// Std headers
#include <memory>
#include <string>

// Forward declarations
struct SDL_Window;

namespace exodus {
// Forward declarations
class Engine;

/// Represents the main application that manages SDL, OpenGL, and the game loop.
class Application {
 public:
  /// Construct the application.
  ///
  /// @param title - The window title.
  /// @param width - The window width.
  /// @param height - The window height.
  explicit Application(std::string title, int width, int height);

  /// Destroy the application.
  ~Application();

  /// Deleted copy constructor to prevent copying.
  Application(const Application&) = delete;

  /// Deleted copy assignment operator to prevent copying.
  auto operator=(const Application&) -> Application& = delete;

  /// Deleted move constructor to prevent moving.
  Application(Application&&) = delete;

  /// Deleted move assignment operator to prevent moving.
  auto operator=(Application&&) -> Application& = delete;

  /// Run the application main loop.
  ///
  /// @return Exit code (0 for success, non-zero for error).
  auto run() -> int;

 private:
  /// Initialise SDL and OpenGL.
  ///
  /// @return True if initialisation was successful, false otherwise.
  auto initialise() -> bool;

  /// Process all pending SDL events.
  void process_events();

  /// The window title.
  std::string title_;

  /// The window width.
  int width_;

  /// The window height.
  int height_;

  /// The SDL window.
  SDL_Window* window_;

  /// The OpenGL context.
  void* gl_context_;

  /// The game engine instance.
  std::unique_ptr<Engine> engine_;

  /// Whether the application is running.
  bool running_;
};
}  // namespace exodus
