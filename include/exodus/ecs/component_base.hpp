// Ensure this file is only included once
#pragma once

/// The base class for all components.
struct ComponentBase {
  /// The copy assignment operator.
  auto operator=(const ComponentBase&) -> ComponentBase& = default;

  /// The move assignment operator.
  auto operator=(ComponentBase&&) -> ComponentBase& = default;

  /// The default constructor.
  ComponentBase() = default;

  /// The virtual destructor.
  virtual ~ComponentBase() = default;

  /// The copy constructor.
  ComponentBase(const ComponentBase&) = default;

  /// The move constructor.
  ComponentBase(ComponentBase&&) = default;
};
