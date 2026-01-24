// Stop SDL hijacking main()
#define SDL_MAIN_HANDLED

// Local headers
#include "exodus/application.hpp"

/// The application entry point.
auto main() -> int {
  exodus::Application app("Exodus", 1280, 720);
  return app.run();
}
