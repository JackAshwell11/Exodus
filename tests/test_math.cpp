// Local headers
#include "exodus/math.hpp"
#include "macros.hpp"

namespace exodus {
/// Implements the fixture for the math.hpp tests.
class Vec2Fixture : public testing::Test {
 protected:
  /// A floating-point vector with all components set to zero for testing.
  static constexpr Vec2f zero{0.0F, 0.0F};

  /// A floating-point vector with a unit length along the x-axis for testing.
  static constexpr Vec2f unit_x{1.0F, 0.0F};

  /// A floating-point vector with a unit length along the y-axis for testing.
  static constexpr Vec2f unit_y{0.0F, 1.0F};

  /// A floating-point vector with random values for testing.
  static constexpr Vec2f vec_a{3.0F, 4.0F};

  /// An additional floating-point vector with random values for testing.
  static constexpr Vec2f vec_b{1.0F, 2.0F};

  /// A floating-point vector with random negative values for testing.
  static constexpr Vec2f negative{-2.0F, -3.0F};
};

/// Test that the default constructor initialises to zero.
TEST_F(Vec2Fixture, DefaultConstructorInitialisesToZero) {
  constexpr Vec2f default_vec{};
  ASSERT_EQ(default_vec.x, 0.0F);
  ASSERT_EQ(default_vec.y, 0.0F);
}

/// Test that the parameterised constructor sets components correctly.
TEST_F(Vec2Fixture, ParameterisedConstructorSetsComponents) {
  constexpr Vec2f parameterised_vec{3.5F, 7.2F};
  ASSERT_EQ(parameterised_vec.x, 3.5F);
  ASSERT_EQ(parameterised_vec.y, 7.2F);
}

/// Test that the conversion constructor from a different type works correctly.
TEST_F(Vec2Fixture, ConversionConstructorFromDifferentType) {
  constexpr Vec2i int_vec{5, 10};
  constexpr Vec2f float_vec{int_vec};
  ASSERT_EQ(float_vec.x, 5.0F);
  ASSERT_EQ(float_vec.y, 10.0F);
}

/// Test that a conversion from float to int truncates values.
TEST_F(Vec2Fixture, ConversionFloatToIntTruncates) {
  constexpr Vec2f float_vec{3.7F, 8.9F};
  constexpr Vec2i int_vector{float_vec};
  ASSERT_EQ(int_vector.x, 3);
  ASSERT_EQ(int_vector.y, 8);
}

/// Test that the equality operator returns true for equal vectors.
TEST_F(Vec2Fixture, EqualityOperatorReturnsTrueForEqualVectors) {
  constexpr Vec2f vec_one{2.0F, 3.0F};
  constexpr Vec2f vec_two{2.0F, 3.0F};
  ASSERT_TRUE(vec_one == vec_two);
}

/// Test that equality operator returns false for different vectors.
TEST_F(Vec2Fixture, EqualityOperatorReturnsFalseForDifferentVectors) { ASSERT_FALSE(vec_a == vec_b); }

/// Test that inequality operator returns true for different vectors.
TEST_F(Vec2Fixture, InequalityOperatorReturnsTrueForDifferentVectors) { ASSERT_TRUE(vec_a != vec_b); }

/// Test that inequality operator returns false for equal vectors.
TEST_F(Vec2Fixture, InequalityOperatorReturnsFalseForEqualVectors) {
  constexpr Vec2f vec_one{2.0F, 3.0F};
  constexpr Vec2f vec_two{2.0F, 3.0F};
  ASSERT_FALSE(vec_one != vec_two);
}

/// Test that vector addition with the same type works correctly.
TEST_F(Vec2Fixture, VectorAdditionSameType) {
  constexpr Vec2f result{vec_a + vec_b};
  ASSERT_EQ(result.x, 4.0F);
  ASSERT_EQ(result.y, 6.0F);
}

/// Test that vector addition with a different type works correctly.
TEST_F(Vec2Fixture, VectorAdditionDifferentType) {
  constexpr Vec2f float_vec(2.5F, 3.5F);
  constexpr Vec2i int_vec{1, 2};
  constexpr Vec2f result{float_vec + int_vec};
  ASSERT_EQ(result.x, 3.5F);
  ASSERT_EQ(result.y, 5.5F);
}

/// Test that scalar addition works correctly.
TEST_F(Vec2Fixture, ScalarAddition) {
  constexpr Vec2f result{vec_a + 2.0F};
  ASSERT_EQ(result.x, 5.0F);
  ASSERT_EQ(result.y, 6.0F);
}

/// Test that the addition assignment operator works correctly.
TEST_F(Vec2Fixture, AdditionAssignmentOperator) {
  Vec2f temp_vec{vec_a};
  temp_vec += vec_b;
  ASSERT_EQ(temp_vec.x, 4.0F);
  ASSERT_EQ(temp_vec.y, 6.0F);
}

/// Test that addition with a zero vector works correctly.
TEST_F(Vec2Fixture, AdditionWithZeroVector) {
  constexpr Vec2f result{vec_a + zero};
  ASSERT_EQ(result, vec_a);
}

/// Test that vector subtraction with the same type works correctly.
TEST_F(Vec2Fixture, VectorSubtractionSameType) {
  constexpr Vec2f result{vec_a - vec_b};
  ASSERT_EQ(result.x, 2.0F);
  ASSERT_EQ(result.y, 2.0F);
}

/// Test that vector subtraction with a different type works correctly.
TEST_F(Vec2Fixture, VectorSubtractionDifferentType) {
  constexpr Vec2f float_vec{5.5F, 7.5F};
  constexpr Vec2i int_vec{2, 3};
  constexpr Vec2f result{float_vec - int_vec};
  ASSERT_EQ(result.x, 3.5F);
  ASSERT_EQ(result.y, 4.5F);
}

/// Test that scalar subtraction works correctly.
TEST_F(Vec2Fixture, ScalarSubtraction) {
  constexpr Vec2f result{vec_a - 1.0F};
  ASSERT_EQ(result.x, 2.0F);
  ASSERT_EQ(result.y, 3.0F);
}

/// Test that the subtraction assignment operator works correctly.
TEST_F(Vec2Fixture, SubtractionAssignmentOperator) {
  Vec2f temp_vec{vec_a};
  temp_vec -= vec_b;
  ASSERT_EQ(temp_vec.x, 2.0F);
  ASSERT_EQ(temp_vec.y, 2.0F);
}

/// Test that subtraction with a zero vector works correctly.
TEST_F(Vec2Fixture, SubtractionWithZeroVector) {
  constexpr Vec2f result{vec_a - zero};
  ASSERT_EQ(result, vec_a);
}

/// Test that scalar multiplication works correctly.
TEST_F(Vec2Fixture, ScalarMultiplication) {
  constexpr Vec2f result{vec_a * 2.0F};
  ASSERT_EQ(result.x, 6.0F);
  ASSERT_EQ(result.y, 8.0F);
}

/// Test that component-wise vector multiplication with a different type works correctly.
TEST_F(Vec2Fixture, ComponentWiseVectorMultiplication) {
  constexpr Vec2f float_vec{2.0F, 3.0F};
  constexpr Vec2i int_vec{4, 5};
  constexpr Vec2f result{float_vec * int_vec};
  ASSERT_EQ(result.x, 8.0F);
  ASSERT_EQ(result.y, 15.0F);
}

/// Test that the multiplication assignment operator works correctly.
TEST_F(Vec2Fixture, MultiplicationAssignmentOperator) {
  Vec2f temp_vec{vec_a};
  temp_vec *= 2.0F;
  ASSERT_EQ(temp_vec.x, 6.0F);
  ASSERT_EQ(temp_vec.y, 8.0F);
}

/// Test that multiplying by zero works correctly.
TEST_F(Vec2Fixture, MultiplicationByZero) {
  constexpr Vec2f result{vec_a * 0.0F};
  ASSERT_EQ(result, zero);
}

/// Test that multiplying by one works correctly.
TEST_F(Vec2Fixture, MultiplicationByOne) {
  constexpr Vec2f result{vec_a * 1.0F};
  ASSERT_EQ(result, vec_a);
}

/// Test that multiplication by a negative scalar works correctly.
TEST_F(Vec2Fixture, MultiplicationByNegativeScalar) {
  constexpr Vec2f result{vec_a * -1.0F};
  ASSERT_EQ(result.x, -3.0F);
  ASSERT_EQ(result.y, -4.0F);
}

/// Test that scalar division works correctly.
TEST_F(Vec2Fixture, ScalarDivision) {
  constexpr Vec2f result{vec_a / 2.0F};
  ASSERT_EQ(result.x, 1.5F);
  ASSERT_EQ(result.y, 2.0F);
}

/// Test that component-wise vector division with a different type works correctly.
TEST_F(Vec2Fixture, ComponentWiseVectorDivision) {
  constexpr Vec2f float_vec{12.0F, 20.0F};
  constexpr Vec2i int_vec{3, 4};
  constexpr Vec2f result{float_vec / int_vec};
  ASSERT_EQ(result.x, 4.0F);
  ASSERT_EQ(result.y, 5.0F);
}

/// Test that the division assignment operator works correctly.
TEST_F(Vec2Fixture, DivisionAssignmentOperator) {
  Vec2f temp_vec{vec_a};
  temp_vec /= 2.0F;
  ASSERT_EQ(temp_vec.x, 1.5F);
  ASSERT_EQ(temp_vec.y, 2.0F);
}

/// Test that dividing by one works correctly.
TEST_F(Vec2Fixture, DivisionByOne) {
  constexpr Vec2f result{vec_a / 1.0F};
  ASSERT_EQ(result, vec_a);
}

/// Test that computing the length of a zero vector works correctly.
TEST_F(Vec2Fixture, LengthOfZeroVector) { ASSERT_EQ(zero.length(), 0.0F); }

/// Test that computing the length of a x-axis unit vector works correctly.
TEST_F(Vec2Fixture, LengthOfUnitVectorX) { ASSERT_FLOAT_EQ(unit_x.length(), 1.0F); }

/// Test that computing the length of a y-axis unit vector works correctly.
TEST_F(Vec2Fixture, LengthOfUnitVectorY) { ASSERT_FLOAT_EQ(unit_y.length(), 1.0F); }

/// Test that computing the length of the 3-4-5 triangle vector works correctly.
TEST_F(Vec2Fixture, LengthOf345Triangle) { ASSERT_FLOAT_EQ(vec_a.length(), 5.0F); }

/// Test that computing the length of a negative vector works correctly.
TEST_F(Vec2Fixture, LengthOfNegativeVector) { ASSERT_FLOAT_EQ(negative.length(), std::hypot(2.0F, 3.0F)); }

/// Test that computing the squared length of zero vector works correctly.
TEST_F(Vec2Fixture, LengthSquaredOfZeroVector) { ASSERT_EQ(zero.length_squared(), 0.0F); }

/// Test that computing the squared length of unit vector works correctly.
TEST_F(Vec2Fixture, LengthSquaredOfUnitVector) { ASSERT_FLOAT_EQ(unit_x.length_squared(), 1.0F); }

/// Test that computing the squared length of the 3-4-5 triangle vector works correctly.
TEST_F(Vec2Fixture, LengthSquaredOf345Triangle) { ASSERT_FLOAT_EQ(vec_a.length_squared(), 25.0F); }

/// Test that the squared length matches length squared.
TEST_F(Vec2Fixture, LengthSquaredMatchesLengthSquared) {
  const float length{vec_a.length()};
  ASSERT_FLOAT_EQ(vec_a.length_squared(), length * length);
}

/// Test that the normalisation of a zero vector returns zero.
TEST_F(Vec2Fixture, NormaliseZeroVectorReturnsZero) {
  const Vec2f result{zero.normalise()};
  ASSERT_EQ(result, zero);
}

/// Test that the normalisation of a unit vector returns itself.
TEST_F(Vec2Fixture, NormaliseUnitVectorReturnsItself) {
  const Vec2f result{unit_x.normalise()};
  ASSERT_FLOAT_EQ(result.x, 1.0F);
  ASSERT_FLOAT_EQ(result.y, 0.0F);
}

/// Test that the normalised vector has a length of one.
TEST_F(Vec2Fixture, NormalisedVectorHasLengthOne) {
  const Vec2f result{vec_a.normalise()};
  ASSERT_FLOAT_EQ(result.length(), 1.0F);
}

/// Test that the normalisation preserves direction.
TEST_F(Vec2Fixture, NormalisationPreservesDirection) {
  const Vec2f result{vec_a.normalise()};
  ASSERT_FLOAT_EQ(result.x, 3.0F / 5.0F);
  ASSERT_FLOAT_EQ(result.y, 4.0F / 5.0F);
}

/// Test that the normalisation of a negative vector works correctly.
TEST_F(Vec2Fixture, NormalisationOfNegativeVector) {
  const Vec2f result{negative.normalise()};
  ASSERT_FLOAT_EQ(result.length(), 1.0F);
  ASSERT_LT(result.x, 0.0F);
  ASSERT_LT(result.y, 0.0F);
}

/// Test that computing the sum of a zero vector works correctly.
TEST_F(Vec2Fixture, SumOfZeroVector) { ASSERT_EQ(zero.sum(), 0.0F); }

/// Test that computing the sum of a positive vector works correctly.
TEST_F(Vec2Fixture, SumOfPositiveComponents) { ASSERT_EQ(vec_a.sum(), 7.0F); }

/// Test that computing the sum of a negative vector works correctly.
TEST_F(Vec2Fixture, SumOfNegativeComponents) { ASSERT_EQ(negative.sum(), -5.0F); }

/// Test that computing the sum of a mixed vector works correctly.
TEST_F(Vec2Fixture, SumOfMixedSignComponents) {
  constexpr Vec2f mixed_vec{5.0F, -3.0F};
  ASSERT_EQ(mixed_vec.sum(), 2.0F);
}

/// Test that computing the dot product of two zero vectors returns zero.
TEST_F(Vec2Fixture, DotProductOfZeroVectors) { ASSERT_EQ(zero.dot(zero), 0.0F); }

/// Test that computing the dot product of a vector with a zero vector returns zero.
TEST_F(Vec2Fixture, DotProductWithZeroVector) { ASSERT_EQ(vec_a.dot(zero), 0.0F); }

/// Test that computing the dot product of a vector with a negative vector works correctly.
TEST_F(Vec2Fixture, DotProductWithNegativeVector) {
  Vec2f v1{2.0F, 3.0F};
  Vec2f v2{-2.0F, -3.0F};
  ASSERT_FLOAT_EQ(v1.dot(v2), -13.0F);
}

/// Test that computing the dot product of two orthogonal vectors returns zero.
TEST_F(Vec2Fixture, DotProductOfOrthogonalVectors) { ASSERT_FLOAT_EQ(unit_x.dot(unit_y), 0.0F); }

/// Test that computing the dot product of two parallel vectors works correctly.
TEST_F(Vec2Fixture, DotProductOfParallelVectors) {
  constexpr Vec2f vec_one{2.0F, 3.0F};
  constexpr Vec2f vec_two{4.0F, 6.0F};
  ASSERT_FLOAT_EQ(vec_one.dot(vec_two), 26.0F);
}

/// Test that computing the dot product is commutative.
TEST_F(Vec2Fixture, DotProductIsCommutative) { ASSERT_FLOAT_EQ(vec_a.dot(vec_b), vec_b.dot(vec_a)); }

/// Test that computing the dot product of a vector with itself works correctly.
TEST_F(Vec2Fixture, DotProductOfVectorWithItself) { ASSERT_FLOAT_EQ(vec_a.dot(vec_a), vec_a.length_squared()); }

/// Test that the hash function produces consistent results.
TEST_F(Vec2Fixture, HashFunctionIsConsistent) {
  constexpr std::hash<Vec2f> hasher;
  const size_t hash_one{hasher(vec_a)};
  const size_t hash_two{hasher(vec_a)};
  ASSERT_EQ(hash_one, hash_two);
}

/// Test that the hash function produces different results for different vectors.
TEST_F(Vec2Fixture, HashFunctionDifferentForDifferentVectors) {
  constexpr std::hash<Vec2f> hasher;
  const size_t hash_one{hasher(vec_a)};
  const size_t hash_two{hasher(vec_b)};
  ASSERT_NE(hash_one, hash_two);
}

/// Test that the hash works with integer vectors.
TEST_F(Vec2Fixture, HashWorksWithIntegerVectors) {
  constexpr std::hash<Vec2i> hasher;
  const size_t hash_one{hasher({3, 4})};
  const size_t hash_two{hasher({5, 6})};
  ASSERT_NE(hash_one, hash_two);
}

/// Test that Vec2f's type alias is float.
TEST_F(Vec2Fixture, Vec2fIsFloat) {
  constexpr Vec2f float_vec{};
  ASSERT_TRUE((std::is_same_v<decltype(float_vec.x), float>));
}

/// Test that Vec2i's type alias is int.
TEST_F(Vec2Fixture, Vec2iIsInt) {
  constexpr Vec2i int_vec{};
  ASSERT_TRUE((std::is_same_v<decltype(int_vec.x), int>));
}

/// Test that Vec2d's type alias is double.
TEST_F(Vec2Fixture, Vec2dIsDouble) {
  constexpr Vec2d double_vec{};
  ASSERT_TRUE((std::is_same_v<decltype(double_vec.x), double>));
}

/// Test that Vec2u64's type alias is uint64_t.
TEST_F(Vec2Fixture, Vec2u64IsUint64) {
  constexpr Vec2u64 unsigned_64_vec{};
  ASSERT_TRUE((std::is_same_v<decltype(unsigned_64_vec.x), std::uint64_t>));
}
}  // namespace exodus
