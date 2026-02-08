// Ensure this file is only included once
#pragma once

namespace exodus {
class Camera;

namespace ecs {
class Registry;

namespace systems {
/// Handle sprite rendering to the screen with the provided camera.
///
/// @param registry The registry to operate on.
/// @param camera The camera to render with.
void sprite_render_system(Registry& registry, const Camera& camera);
}  // namespace systems
}  // namespace ecs
}  // namespace exodus
