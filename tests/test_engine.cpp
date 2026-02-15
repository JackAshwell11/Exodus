// Local headers
#include "exodus/engine.hpp"
#include "macros.hpp"
#include "test_renderer.hpp"

namespace exodus {
/// Test that the engine constructor creates an engine without throwing.
TEST(EngineTest, ConstructorCreatesEngineWithoutThrowing) { ASSERT_NO_THROW(Engine engine); }

/// Test that the update method runs without throwing.
TEST(EngineTest, UpdateRunsWithoutThrowing) {
  Engine engine;
  ASSERT_NO_THROW(engine.update(1.0F));
}

/// Test that the fixed_update method runs without throwing.
TEST(EngineTest, FixedUpdateRunsWithoutThrowing) {
  Engine engine;
  ASSERT_NO_THROW(engine.fixed_update(1.0F));
}

/// Test that the render method runs without throwing.
TEST(EngineTest, RenderRunsWithoutThrowing) {
  Engine engine;
  rendering::TestRenderer renderer;
  ASSERT_NO_THROW(engine.render(1.0F, renderer));
  ASSERT_TRUE(renderer.flush_called);
}

/// Test that multiple updates run without throwing.
TEST(EngineTest, MultipleUpdatesRunWithoutThrowing) {
  Engine engine;
  for (int i{0}; i < 10; i++) {
    ASSERT_NO_THROW(engine.update(1.0F));
  }
}

/// Test that multiple fixed_updates run without throwing.
TEST(EngineTest, MultipleFixedUpdatesRunWithoutThrowing) {
  Engine engine;
  for (int i{0}; i < 10; i++) {
    ASSERT_NO_THROW(engine.fixed_update(1.0F));
  }
}

/// Test that multiple renders run without throwing.
TEST(EngineTest, MultipleRendersRunWithoutThrowing) {
  Engine engine;
  rendering::TestRenderer renderer;
  for (int i{0}; i < 10; i++) {
    renderer.flush_called = false;
    ASSERT_NO_THROW(engine.render(1.0F, renderer));
    ASSERT_TRUE(renderer.flush_called);
  }
}
}  // namespace exodus
