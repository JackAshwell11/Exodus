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

  /// Construct a 2D vector from another vector with a different component type.
  ///
  /// @tparam U The numeric type of the source vector.
  /// @param other The source vector to convert from.
  template <typename U>
  constexpr explicit Vec2(const Vec2<U>& other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

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

  /// Adds a vector of a different type to this vector.
  ///
  /// @tparam U The numeric type of the other vector.
  /// @param other The vector to add to this vector.
  /// @return A new vector representing the sum, with the result type being T.
  template <typename U>
  constexpr auto operator+(const Vec2<U>& other) const noexcept -> Vec2 {
    return {x + static_cast<T>(other.x), y + static_cast<T>(other.y)};
  }

  /// Adds a scalar value to each component of the vector.
  ///
  /// @param scalar The scalar value to add.
  /// @return A new vector with the scalar added
  constexpr auto operator+(const T& scalar) const noexcept -> Vec2 { return {x + scalar, y + scalar}; }

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

  /// Subtracts a vector of a different type from this vector.
  ///
  /// @tparam U The numeric type of the other vector.
  /// @param other The vector to subtract from this vector.
  /// @return A new vector representing the difference, with the result type being T.
  template <typename U>
  constexpr auto operator-(const Vec2<U>& other) const noexcept -> Vec2 {
    return {x - static_cast<T>(other.x), y - static_cast<T>(other.y)};
  }

  /// Subtracts a scalar value from each component of the vector.
  ///
  /// @param scalar The scalar value to subtract.
  /// @return A new vector with the scalar subtracted.
  constexpr auto operator-(const T& scalar) const noexcept -> Vec2 { return {x - scalar, y - scalar}; }

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

  /// Multiplies this vector by another vector component-wise.
  ///
  /// @tparam U The numeric type of the other vector.
  /// @param other The vector to multiply with this vector.
  /// @return A new vector representing the component-wise product.
  template<typename U>
  constexpr auto operator*(const Vec2<U>& other) const noexcept -> Vec2 { return {x * static_cast<T>(other.x), y * static_cast<T>(other.y)}; }

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

  /// Divides this vector by another vector component-wise.
  ///
  /// @tparam U The numeric type of the other vector.
  /// @param other The vector to divide this vector by.
  /// @return A new vector representing the component-wise division.
  template<typename U>
  constexpr auto operator/(const Vec2<U>& other) const noexcept -> Vec2 { return {x / static_cast<T>(other.x), y / static_cast<T>(other.y)}; }

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
  [[nodiscard]] constexpr auto length() const noexcept -> T { return std::hypot(x, y); }

  /// Calculate the squared length of the vector.
  ///
  /// @return The squared length of the vector.
  [[nodiscard]] constexpr auto length_squared() const noexcept -> T { return (x * x) + (y * y); }

  /// Normalise the vector to have a length of 1.
  ///
  /// @return A new vector that is the normalised version of this vector.
  [[nodiscard]] constexpr auto normalise() const noexcept -> Vec2 {
    const T len{length()};
    if (len == 0) {
      return {0, 0};
    }
    return {x / len, y / len};
  }

  /// Calculate the sum of the x and y components.
  ///
  /// @return The sum of x and y.
  [[nodiscard]] constexpr auto sum() const noexcept -> T { return x + y; }

  /// Calculate the dot product of this vector with another vector.
  ///
  /// @param other The vector to compute the dot product with.
  /// @return The dot product as a scalar value.
  [[nodiscard]] constexpr auto dot(const Vec2& other) const noexcept -> T { return (x * other.x) + (y * other.y); }
};

// Type aliases for common vector types
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2d = Vec2<double>;
using Vec2u64 = Vec2<std::uint64_t>;
}  // namespace exodus
