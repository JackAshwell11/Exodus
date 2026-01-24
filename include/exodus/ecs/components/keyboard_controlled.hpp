// Ensure this file is only included once
#pragma once

// Local headers
#include "exodus/ecs/component_base.hpp"

namespace exodus::ecs::components {
/// Allows a game object to be controlled by keyboard input.
struct KeyboardControlled final : ComponentBase {
  /// Allow using the constructor from ComponentBase.
  using ComponentBase::ComponentBase;
};
}  // namespace exodus::ecs::components
