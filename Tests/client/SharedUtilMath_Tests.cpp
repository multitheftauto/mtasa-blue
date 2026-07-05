/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SharedUtilMath_Tests.cpp
 *  PURPOSE:     Google Test suite for SharedUtil.Math.h utilities
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Math.h>

static constexpr float kPi = 3.14159265f;

///////////////////////////////////////////////////////////////
//
// Square
//
///////////////////////////////////////////////////////////////

// Verify Square() works for positive, negative, and zero integers
TEST(SharedUtilMath, SquareInt)
{
    EXPECT_EQ(SharedUtil::Square(5), 25);
    EXPECT_EQ(SharedUtil::Square(-3), 9);
    EXPECT_EQ(SharedUtil::Square(0), 0);
}

// Verify Square() works for floating-point values
TEST(SharedUtilMath, SquareFloat)
{
    EXPECT_FLOAT_EQ(SharedUtil::Square(2.5f), 6.25f);
}

///////////////////////////////////////////////////////////////
//
// DegreesToRadians
//
///////////////////////////////////////////////////////////////

// Verify 0 degrees converts to 0 radians
TEST(SharedUtilMath, DegreesToRadiansZero)
{
    EXPECT_FLOAT_EQ(SharedUtil::DegreesToRadians(0.0f), 0.0f);
}

// Verify 180 degrees converts to pi radians
TEST(SharedUtilMath, DegreesToRadians180)
{
    EXPECT_NEAR(SharedUtil::DegreesToRadians(180.0f), kPi, 0.001f);
}

// Verify 90 degrees converts to pi/2 radians
TEST(SharedUtilMath, DegreesToRadians90)
{
    EXPECT_NEAR(SharedUtil::DegreesToRadians(90.0f), kPi / 2.0f, 0.001f);
}

// Verify 360 degrees converts to 2*pi radians
TEST(SharedUtilMath, DegreesToRadians360)
{
    EXPECT_NEAR(SharedUtil::DegreesToRadians(360.0f), 2.0f * kPi, 0.01f);
}

///////////////////////////////////////////////////////////////
//
// ShouldUseInt
//
///////////////////////////////////////////////////////////////

// Verify an exact integer double is recognized and output correctly
TEST(SharedUtilMath, ShouldUseIntExactInteger)
{
    int number = 0;
    EXPECT_TRUE(SharedUtil::ShouldUseInt(42.0, &number));
    EXPECT_EQ(number, 42);
}

// Verify a fractional double is rejected
TEST(SharedUtilMath, ShouldUseIntFractionalValue)
{
    int number = 0;
    EXPECT_FALSE(SharedUtil::ShouldUseInt(42.5, &number));
}

// Verify 0.0 is recognized as an integer
TEST(SharedUtilMath, ShouldUseIntZero)
{
    int number = -1;
    EXPECT_TRUE(SharedUtil::ShouldUseInt(0.0, &number));
    EXPECT_EQ(number, 0);
}

// Verify large-but-representable integer values are accepted via rounding
TEST(SharedUtilMath, ShouldUseIntLargeValue)
{
    // Values in the large-int range (>= 0x1000000) should return true via rounding
    int    number = 0;
    double large = 20000000.0;
    EXPECT_TRUE(SharedUtil::ShouldUseInt(large, &number));
    EXPECT_EQ(number, 20000000);
}

// Verify values exceeding int range are rejected
TEST(SharedUtilMath, ShouldUseIntTooLargeForInt)
{
    int    number = 0;
    double huge = 3e10;
    EXPECT_FALSE(SharedUtil::ShouldUseInt(huge, &number));
}

// Verify negative exact integers are accepted
TEST(SharedUtilMath, ShouldUseIntNegative)
{
    int number = 0;
    EXPECT_TRUE(SharedUtil::ShouldUseInt(-100.0, &number));
    EXPECT_EQ(number, -100);
}

///////////////////////////////////////////////////////////////
//
// IsNearlyEqual
//
///////////////////////////////////////////////////////////////

// Verify identical values are nearly equal
TEST(SharedUtilMath, IsNearlyEqualSameValue)
{
    EXPECT_TRUE(SharedUtil::IsNearlyEqual(1.0f, 1.0f));
}

// Verify values within the epsilon threshold are considered nearly equal
TEST(SharedUtilMath, IsNearlyEqualWithinEpsilon)
{
    float eps = 0.01f;
    EXPECT_TRUE(SharedUtil::IsNearlyEqual(1.0f, 1.005f, eps));
}

// Verify values outside the epsilon threshold are not nearly equal
TEST(SharedUtilMath, IsNearlyEqualOutsideEpsilon)
{
    float eps = 0.001f;
    EXPECT_FALSE(SharedUtil::IsNearlyEqual(1.0f, 1.01f, eps));
}

///////////////////////////////////////////////////////////////
//
// ApproximatelyEqual (relative comparison using max)
//
///////////////////////////////////////////////////////////////

// Verify identical values pass the max-relative-error approximation check
TEST(SharedUtilMath, ApproximatelyEqualSameValue)
{
    EXPECT_TRUE(SharedUtil::ApproximatelyEqual(100.0f, 100.0f));
}

// Verify values within the relative epsilon (based on max) are approximately equal
TEST(SharedUtilMath, ApproximatelyEqualLargeEpsilon)
{
    // With epsilon=0.1, 100 and 105 differ by 5, max(100,105)=105, 105*0.1=10.5 > 5
    EXPECT_TRUE(SharedUtil::ApproximatelyEqual(100.0f, 105.0f, 0.1f));
}

// Verify values outside the relative epsilon fail the approximation check
TEST(SharedUtilMath, ApproximatelyEqualFailsSmallEpsilon)
{
    EXPECT_FALSE(SharedUtil::ApproximatelyEqual(100.0f, 110.0f, 0.01f));
}

///////////////////////////////////////////////////////////////
//
// EssentiallyEqual (relative comparison using min)
//
///////////////////////////////////////////////////////////////

// Verify identical values pass the min-relative-error essential equality check
TEST(SharedUtilMath, EssentiallyEqualSameValue)
{
    EXPECT_TRUE(SharedUtil::EssentiallyEqual(100.0f, 100.0f));
}

// Verify EssentiallyEqual uses min() for a stricter relative comparison than ApproximatelyEqual
TEST(SharedUtilMath, EssentiallyEqualIsStricterThanApproximately)
{
    // With epsilon=0.1, 100 and 105: min(100,105)=100, 100*0.1=10 > 5 => true
    EXPECT_TRUE(SharedUtil::EssentiallyEqual(100.0f, 105.0f, 0.1f));
    // But with a smaller epsilon, EssentiallyEqual should be stricter
    // min(100,110)=100, 100*0.01=1 < 10 => false
    EXPECT_FALSE(SharedUtil::EssentiallyEqual(100.0f, 110.0f, 0.01f));
}

///////////////////////////////////////////////////////////////
//
// DefinitelyGreaterThan / DefinitelyLessThan
//
///////////////////////////////////////////////////////////////

// Verify a clearly larger value is detected as definitely greater
TEST(SharedUtilMath, DefinitelyGreaterThanClearlyGreater)
{
    EXPECT_TRUE(SharedUtil::DefinitelyGreaterThan(200.0f, 100.0f, 0.1f));
}

// Verify equal values are not definitely greater than each other
TEST(SharedUtilMath, DefinitelyGreaterThanNearlyEqual)
{
    // 100.0 vs 100.0 — not definitely greater
    EXPECT_FALSE(SharedUtil::DefinitelyGreaterThan(100.0f, 100.0f, 0.1f));
}

// Verify a clearly smaller value is detected as definitely less
TEST(SharedUtilMath, DefinitelyLessThanClearlyLess)
{
    EXPECT_TRUE(SharedUtil::DefinitelyLessThan(100.0f, 200.0f, 0.1f));
}

// Verify equal values are not definitely less than each other
TEST(SharedUtilMath, DefinitelyLessThanNearlyEqual)
{
    EXPECT_FALSE(SharedUtil::DefinitelyLessThan(100.0f, 100.0f, 0.1f));
}

///////////////////////////////////////////////////////////////
//
// NumberOfSignificantBits
//
///////////////////////////////////////////////////////////////

// Verify compile-time bit count for 0 is 0
TEST(SharedUtilMath, NumberOfSignificantBitsZero)
{
    EXPECT_EQ(SharedUtil::NumberOfSignificantBits<0>::COUNT, 0);
}

// Verify compile-time bit count for 1 is 1
TEST(SharedUtilMath, NumberOfSignificantBitsOne)
{
    EXPECT_EQ(SharedUtil::NumberOfSignificantBits<1>::COUNT, 1);
}

// Verify 255 (0xFF) requires 8 significant bits
TEST(SharedUtilMath, NumberOfSignificantBits255)
{
    EXPECT_EQ(SharedUtil::NumberOfSignificantBits<255>::COUNT, 8);
}

// Verify 256 (0x100) requires 9 significant bits
TEST(SharedUtilMath, NumberOfSignificantBits256)
{
    EXPECT_EQ(SharedUtil::NumberOfSignificantBits<256>::COUNT, 9);
}
