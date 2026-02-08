// Ensure this file is only included once
#pragma once

namespace exodus {
class Camera;

namespace ecs {
class Registry;

namespace systems {
/// Handle updating the camera position to follow the player.
///
/// @param registry The registry to operate on.
/// @param camera The camera to update the position of.
void camera_follow_system(Registry& registry, Camera& camera);
}  // namespace systems
}  // namespace ecs
}  // namespace exodus
