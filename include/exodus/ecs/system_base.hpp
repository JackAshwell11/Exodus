// Ensure this file is only included once
#pragma once

// Forward declarations
class Registry;

/// The base class for all systems.
class SystemBase {
 public:
  /// The copy assignment operator.
  auto operator=(const SystemBase&) -> SystemBase& = default;

  /// The move assignment operator.
  auto operator=(SystemBase&&) -> SystemBase& = default;

  /// Initialise the object.
  ///
  /// @param registry - The registry that manages the game objects, components, and systems.
  explicit SystemBase(Registry* registry) : registry_(registry) {}

  /// The virtual destructor.
  virtual ~SystemBase() = default;

  /// The copy constructor.
  SystemBase(const SystemBase&) = default;

  /// The move constructor.
  SystemBase(SystemBase&&) = default;

  /// Get the registry that manages the game objects, components, and systems.
  ///
  /// @return The registry that manages the game objects, components, and systems.
  [[nodiscard]] auto get_registry() const -> Registry* { return registry_; }

  /// Process update logic for a system.
  virtual void update(double /*delta_time*/) const {}

 private:
  /// The registry that manages the game objects, components, and systems.
  Registry* registry_;
};
