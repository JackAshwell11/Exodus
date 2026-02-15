// Ensure this file is only included once
#pragma once

namespace exodus {
class Camera;

namespace rendering {
class Renderer;
}  // namespace rendering

namespace ecs {
class Registry;

namespace systems {
/// Handle sprite rendering to the screen with the provided camera.
///
/// @param registry The registry to operate on.
/// @param camera The camera to render with.
/// @param renderer The renderer to use for drawing to the screen.
void sprite_render_system(Registry& registry, const Camera& camera, rendering::Renderer& renderer);
}  // namespace systems
}  // namespace ecs
}  // namespace exodus
