// Ensure this file is only included once
#pragma once

// Forward declarations
class Registry;

namespace exodus {
class Camera;

namespace ecs {
/// The base class for all systems.
class SystemBase {
 public:
  /// Construct the system.
  SystemBase() : registry_(nullptr) {}

  /// Construct the system with a registry.
  ///
  /// @param registry The registry that manages the game objects, components, and systems.
  explicit SystemBase(Registry* registry) : registry_(registry) {}

  /// Destroy the system.
  virtual ~SystemBase() = default;

  /// Deleted copy constructor to prevent object slicing.
  SystemBase(const SystemBase&) = delete;

  /// Deleted copy assignment to prevent object slicing.
  auto operator=(const SystemBase&) -> SystemBase& = delete;

  /// Deleted move constructor to prevent object slicing.
  SystemBase(SystemBase&&) = delete;

  /// Deleted move assignment to prevent object slicing.
  auto operator=(SystemBase&&) -> SystemBase& = delete;

  /// Process update logic for a system.
  virtual void update(double /*delta_time*/) const {}

  /// Process fixed update logic for a system.
  virtual void fixed_update(double /*delta_time*/) const {}

  /// Process rendering logic for a system.
  virtual void render(const Camera& /*camera*/) const {}

 protected:
  /// The registry that manages the game objects, components, and systems.
  Registry* registry_;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
};
}  // namespace ecs
}  // namespace exodus
