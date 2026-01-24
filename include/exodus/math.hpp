// Ensure this file is only included once
#pragma once

// Std headers
#include <cmath>

namespace exodus {
/// A 2D vector structure for positions, directions, and sizes.
///
/// @tparam T The numeric type for the vector components.
template <typename T>
struct Vec2 {
  /// The x component of the vector.
  T x;

  /// The y component of the vector.
  T y;

  /// Construct a zero 2D vector.
  constexpr Vec2() noexcept : x(0), y(0) {}

  /// Construct a 2D vector with the specified x and y components.
  ///
  /// @param x The x component of the vector.
  /// @param y The y component of the vector.
  constexpr Vec2(T x, T y) noexcept : x(x), y(y) {}

  /// Checks if two vectors are equal.
  ///
  /// @param other The vector to compare with.
  /// @return True if both components are equal, false otherwise.
  constexpr auto operator==(const Vec2& other) const noexcept -> bool { return x == other.x && y == other.y; }

  /// Checks if two vectors are not equal.
  ///
  /// @param other The vector to compare with.
  /// @return True if any component is different, false otherwise.
  constexpr auto operator!=(const Vec2& other) const noexcept -> bool { return !(*this == other); }

  /// Adds two vectors component-wise.
  ///
  /// @param other The vector to add to this vector.
  /// @return A new vector representing the sum of the two vectors.
  constexpr auto operator+(const Vec2& other) const noexcept -> Vec2 { return {x + other.x, y + other.y}; }

  /// Adds another vector to this vector in-place.
  ///
  /// @param other The vector to add to this vector.
  /// @return This vector after the addition.
  constexpr auto operator+=(const Vec2& other) noexcept -> Vec2& {
    x += other.x;
    y += other.y;
    return *this;
  }

  /// Subtracts one vector from another component-wise.
  ///
  /// @param other The vector to subtract from this vector.
  /// @return A new vector representing the difference of the two vectors.
  constexpr auto operator-(const Vec2& other) const noexcept -> Vec2 { return {x - other.x, y - other.y}; }

  /// Subtracts another vector from this vector in-place.
  ///
  /// @param other The vector to subtract from this vector.
  /// @return This vector after the subtraction.
  constexpr auto operator-=(const Vec2& other) noexcept -> Vec2& {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  /// Multiplies the vector by a scalar value.
  ///
  /// @param scalar The scalar value to multiply each component by.
  /// @return A new vector with each component multiplied by the scalar.
  constexpr auto operator*(T scalar) const noexcept -> Vec2 { return {x * scalar, y * scalar}; }

  /// Multiplies this vector by a scalar value in-place.
  ///
  /// @param scalar The scalar value to multiply each component by.
  /// @return This vector after the multiplication.
  constexpr auto operator*=(T scalar) noexcept -> Vec2& {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  /// Divides the vector by a scalar value.
  ///
  /// @param scalar The scalar value to divide each component by.
  /// @return A new vector with each component divided by the scalar.
  constexpr auto operator/(T scalar) const noexcept -> Vec2 { return {x / scalar, y / scalar}; }

  /// Divides this vector by a scalar value in-place.
  ///
  /// @param scalar The scalar value to divide each component by.
  /// @return This vector after the division.
  constexpr auto operator/=(T scalar) noexcept -> Vec2& {
    x /= scalar;
    y /= scalar;
    return *this;
  }

  /// Calculate the length (magnitude) of the vector.
  ///
  /// @return The length of the vector.
  [[nodiscard]] auto length() const noexcept -> T {
    return std::hypot(x, y);
  }

  /// Normalise the vector to have a length of 1.
  ///
  /// @return A new vector that is the normalised version of this vector.
  [[nodiscard]] auto normalise() const noexcept -> Vec2 {
    const T len = length();
    if (len == 0) {
      return {0, 0};
    }
    return {x / len, y / len};
  }
};

// Type aliases for common vector types
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2d = Vec2<double>;
}  // namespace exodus
