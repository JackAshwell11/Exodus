// Stop SDL hijacking main()
#define SDL_MAIN_HANDLED

// Local headers
#include "exodus/application.hpp"

namespace {
/// The size of the screen.
constexpr exodus::Vec2f SCREEN_SIZE{1280.0F, 720.0F};
}  // namespace

/// The application entry point.
auto main() -> int {
  exodus::Application app("Exodus", SCREEN_SIZE);
  return app.run();
}
