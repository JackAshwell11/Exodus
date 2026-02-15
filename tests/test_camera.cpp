// Local headers
#include "exodus/camera.hpp"
#include "macros.hpp"

namespace exodus {
/// Implements the fixture for the camera.hpp tests.
class CameraFixture : public testing::Test {
 protected:
  /// The camera for testing.
  Camera camera;
};

/// Test that initialising the camera without a position sets the default position and zoom level correctly.
TEST_F(CameraFixture, DefaultConstructor) {
  ASSERT_EQ(camera.get_position().x, 0.0F);
  ASSERT_EQ(camera.get_position().y, 0.0F);
  ASSERT_EQ(camera.get_zoom(), 1.0F);
}

/// Test that initialising the camera with a position sets the position correctly.
TEST_F(CameraFixture, ConstructorWithPosition) {
  const Camera cam{{1.0F, 2.0F}};
  ASSERT_EQ(cam.get_position().x, 1.0F);
  ASSERT_EQ(cam.get_position().y, 2.0F);
  ASSERT_EQ(cam.get_zoom(), 1.0F);
}

/// Test that setting the position updates the position correctly.
TEST_F(CameraFixture, SetPosition) {
  camera.set_position({3.0F, 4.0F});
  ASSERT_EQ(camera.get_position().x, 3.0F);
  ASSERT_EQ(camera.get_position().y, 4.0F);
}

/// Test that moving updates the position by the offset.
TEST_F(CameraFixture, Move) {
  camera.move({1.0F, 1.0F});
  ASSERT_EQ(camera.get_position().x, 1.0F);
  ASSERT_EQ(camera.get_position().y, 1.0F);
  camera.move({-0.5F, 2.0F});
  ASSERT_EQ(camera.get_position().x, 0.5F);
  ASSERT_EQ(camera.get_position().y, 3.0F);
}

/// Test that setting the zoom updates the zoom level.
TEST_F(CameraFixture, SetZoom) {
  camera.set_zoom(2.0F);
  ASSERT_EQ(camera.get_zoom(), 2.0F);
}

/// Test that getting the zoom returns the correct zoom level.
TEST_F(CameraFixture, GetZoom) {
  ASSERT_EQ(camera.get_zoom(), 1.0F);
  camera.set_zoom(0.5F);
  ASSERT_EQ(camera.get_zoom(), 0.5F);
}
}  // namespace exodus
