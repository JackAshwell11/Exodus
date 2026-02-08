// Ensure this file is only included once
#pragma once

namespace exodus::ecs {
class Registry;

namespace systems {
/// Handle keyboard input and updates velocity for keyboard-controlled game objects.
///
/// @param registry The registry to operate on.
void input_system(Registry& registry);
}  // namespace systems
}  // namespace exodus::ecs
