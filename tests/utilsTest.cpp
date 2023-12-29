#include "utils.h"
#include <gtest/gtest.h>

// Test Vector addition
TEST(VectorUtilsTest, VectorAddition) {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    Vector expected = {5.0, 7.0, 9.0};
    EXPECT_EQ(v1 + v2, expected);
}

// Test Vector subtraction
TEST(VectorUtilsTest, VectorSubtraction) {
    Vector v1 = {4.0, 5.0, 6.0};
    Vector v2 = {1.0, 2.0, 3.0};
    Vector expected = {3.0, 3.0, 3.0};
    EXPECT_EQ(v1 - v2, expected);
}

// Test Vector multiplication (element-wise)
TEST(VectorUtilsTest, VectorMultiplication) {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    Vector expected = {4.0, 10.0, 18.0};
    EXPECT_EQ(v1 * v2, expected);
}

// Test Vector-scalar multiplication (left and right)
TEST(VectorUtilsTest, VectorScalarMultiplication) {
    Vector v1 = {1.0, 2.0, 3.0};
    double scalar = 2.0;
    Vector expected = {2.0, 4.0, 6.0};
    EXPECT_EQ(v1 * scalar, expected);
    EXPECT_EQ(scalar * v1, expected);
}

// Test dot product of two Vectors
TEST(VectorUtilsTest, DotProduct) {
    Vector v1 = {1.0, 2.0, 3.0};
    Vector v2 = {4.0, 5.0, 6.0};
    double expected = 32.0; // 1*4 + 2*5 + 3*6
    EXPECT_EQ(dotProduct(v1, v2), expected);
}

// Test Matrix transpose
TEST(MatrixUtilsTest, MatrixTranspose) {
    Matrix mx1 = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
    Matrix expected = {{1.0, 4.0}, {2.0, 5.0}, {3.0, 6.0}};
    EXPECT_EQ(transpose(mx1), expected);
}

// Test Matrix addition
TEST(MatrixUtilsTest, MatrixAddition) {
    Matrix mx1 = {{1.0, 2.0}, {3.0, 4.0}};
    Matrix mx2 = {{5.0, 6.0}, {7.0, 8.0}};
    Matrix expected = {{6.0, 8.0}, {10.0, 12.0}};
    EXPECT_EQ(mx1 + mx2, expected);
}

// Test Matrix subtraction
TEST(MatrixUtilsTest, MatrixSubtraction) {
    Matrix mx1 = {{5.0, 6.0}, {7.0, 8.0}};
    Matrix mx2 = {{1.0, 2.0}, {3.0, 4.0}};
    Matrix expected = {{4.0, 4.0}, {4.0, 4.0}};
    EXPECT_EQ(mx1 - mx2, expected);
}

// Test Matrix multiplication (element-wise)
TEST(MatrixUtilsTest, MatrixMultiplication) {
    Matrix mx1 = {{1.0, 2.0}, {3.0, 4.0}};
    Matrix mx2 = {{5.0, 6.0}, {7.0, 8.0}};
    Matrix expected = {{5.0, 12.0}, {21.0, 32.0}};
    EXPECT_EQ(mx1 * mx2, expected);
}

// Test Matrix-scalar multiplication (left and right)
TEST(MatrixUtilsTest, MatrixScalarMultiplication) {
    Matrix mx1 = {{1.0, 2.0}, {3.0, 4.0}};
    double scalar = 2.0;
    Matrix expected = {{2.0, 4.0}, {6.0, 8.0}};
    //EXPECT_EQ(mx1 * scalar, expected);
    EXPECT_EQ(scalar * mx1, expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}