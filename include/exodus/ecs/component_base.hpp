// Ensure this file is only included once
#pragma once

namespace exodus::ecs {
/// The base class for all components.
struct ComponentBase {
  /// Construct a component.
  ComponentBase() = default;

  /// Destroy the component.
  virtual ~ComponentBase() = default;

  /// Deleted copy constructor to prevent object slicing.
  ComponentBase(const ComponentBase&) = delete;

  /// Deleted copy assignment to prevent object slicing.
  auto operator=(const ComponentBase&) -> ComponentBase& = delete;

  /// Deleted move constructor to prevent object slicing.
  ComponentBase(ComponentBase&&) = delete;

  /// Deleted move assignment to prevent object slicing.
  auto operator=(ComponentBase&&) -> ComponentBase& = delete;
};
}  // namespace exodus::ecs
