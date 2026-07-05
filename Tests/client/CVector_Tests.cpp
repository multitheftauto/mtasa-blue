/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CVector_Tests.cpp
 *  PURPOSE:     Google Test suite for CVector (3D vector math)
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include <CVector.h>
#include <CVector2D.h>

static constexpr float kEpsilon = 0.001f;

///////////////////////////////////////////////////////////////
//
// Construction
//
///////////////////////////////////////////////////////////////

// Verify default constructor zero-initializes all components
TEST(CVector, DefaultConstructor)
{
    CVector v;
    EXPECT_FLOAT_EQ(v.fX, 0.0f);
    EXPECT_FLOAT_EQ(v.fY, 0.0f);
    EXPECT_FLOAT_EQ(v.fZ, 0.0f);
}

// Verify three-argument constructor sets all components correctly
TEST(CVector, ExplicitConstructor)
{
    CVector v(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v.fX, 1.0f);
    EXPECT_FLOAT_EQ(v.fY, 2.0f);
    EXPECT_FLOAT_EQ(v.fZ, 3.0f);
}

// Verify single-argument constructor sets fX and zero-initializes fY, fZ
TEST(CVector, PartialConstructor)
{
    CVector v(5.0f);
    EXPECT_FLOAT_EQ(v.fX, 5.0f);
    EXPECT_FLOAT_EQ(v.fY, 0.0f);
    EXPECT_FLOAT_EQ(v.fZ, 0.0f);
}

// Verify construction from CVector4D copies XYZ and discards W
TEST(CVector, FromCVector4D)
{
    CVector4D v4(1.0f, 2.0f, 3.0f, 4.0f);
    CVector   v(v4);
    EXPECT_FLOAT_EQ(v.fX, 1.0f);
    EXPECT_FLOAT_EQ(v.fY, 2.0f);
    EXPECT_FLOAT_EQ(v.fZ, 3.0f);
}

///////////////////////////////////////////////////////////////
//
// Length / Normalize
//
///////////////////////////////////////////////////////////////

// Verify Length() returns the Euclidean magnitude (3-4-5 triangle)
TEST(CVector, Length)
{
    CVector v(3.0f, 4.0f, 0.0f);
    EXPECT_NEAR(v.Length(), 5.0f, kEpsilon);
}

// Verify LengthSquared() returns length without the sqrt
TEST(CVector, LengthSquared)
{
    CVector v(3.0f, 4.0f, 0.0f);
    EXPECT_NEAR(v.LengthSquared(), 25.0f, kEpsilon);
}

// Verify Normalize() produces a unit vector and returns the original length
TEST(CVector, Normalize)
{
    CVector v(0.0f, 3.0f, 4.0f);
    float   origLen = v.Normalize();
    EXPECT_NEAR(origLen, 5.0f, kEpsilon);
    EXPECT_NEAR(v.Length(), 1.0f, kEpsilon);
}

// Verify Normalize() on a zero vector returns 0 without crashing
TEST(CVector, NormalizeZeroVector)
{
    CVector v(0.0f, 0.0f, 0.0f);
    float   len = v.Normalize();
    EXPECT_FLOAT_EQ(len, 0.0f);
}

///////////////////////////////////////////////////////////////
//
// Dot / Cross product
//
///////////////////////////////////////////////////////////////

// Verify DotProduct returns the sum of component-wise products
TEST(CVector, DotProduct)
{
    CVector a(1.0f, 2.0f, 3.0f);
    CVector b(4.0f, 5.0f, 6.0f);
    EXPECT_NEAR(a.DotProduct(&b), 32.0f, kEpsilon);
}

// Verify DotProduct of perpendicular unit vectors is zero
TEST(CVector, DotProductPerpendicular)
{
    CVector a(1.0f, 0.0f, 0.0f);
    CVector b(0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(a.DotProduct(&b), 0.0f, kEpsilon);
}

// Verify CrossProduct of X and Y axes produces the Z axis
TEST(CVector, CrossProduct)
{
    CVector a(1.0f, 0.0f, 0.0f);
    CVector b(0.0f, 1.0f, 0.0f);
    a.CrossProduct(&b);
    EXPECT_NEAR(a.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(a.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(a.fZ, 1.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Arithmetic operators
//
///////////////////////////////////////////////////////////////

// Verify operator+ adds components pairwise
TEST(CVector, Addition)
{
    CVector a(1.0f, 2.0f, 3.0f);
    CVector b(4.0f, 5.0f, 6.0f);
    CVector c = a + b;
    EXPECT_FLOAT_EQ(c.fX, 5.0f);
    EXPECT_FLOAT_EQ(c.fY, 7.0f);
    EXPECT_FLOAT_EQ(c.fZ, 9.0f);
}

// Verify operator- subtracts components pairwise
TEST(CVector, Subtraction)
{
    CVector a(4.0f, 5.0f, 6.0f);
    CVector b(1.0f, 2.0f, 3.0f);
    CVector c = a - b;
    EXPECT_FLOAT_EQ(c.fX, 3.0f);
    EXPECT_FLOAT_EQ(c.fY, 3.0f);
    EXPECT_FLOAT_EQ(c.fZ, 3.0f);
}

// Verify unary operator- negates all components
TEST(CVector, Negation)
{
    CVector a(1.0f, -2.0f, 3.0f);
    CVector b = -a;
    EXPECT_FLOAT_EQ(b.fX, -1.0f);
    EXPECT_FLOAT_EQ(b.fY, 2.0f);
    EXPECT_FLOAT_EQ(b.fZ, -3.0f);
}

// Verify scalar multiplication scales all components
TEST(CVector, ScalarMultiply)
{
    CVector a(1.0f, 2.0f, 3.0f);
    CVector b = a * 2.0f;
    EXPECT_FLOAT_EQ(b.fX, 2.0f);
    EXPECT_FLOAT_EQ(b.fY, 4.0f);
    EXPECT_FLOAT_EQ(b.fZ, 6.0f);
}

// Verify scalar division divides all components
TEST(CVector, ScalarDivide)
{
    CVector a(4.0f, 6.0f, 8.0f);
    CVector b = a / 2.0f;
    EXPECT_FLOAT_EQ(b.fX, 2.0f);
    EXPECT_FLOAT_EQ(b.fY, 3.0f);
    EXPECT_FLOAT_EQ(b.fZ, 4.0f);
}

// Verify operator+= modifies the vector in place
TEST(CVector, CompoundAdd)
{
    CVector a(1.0f, 2.0f, 3.0f);
    a += CVector(10.0f, 20.0f, 30.0f);
    EXPECT_FLOAT_EQ(a.fX, 11.0f);
    EXPECT_FLOAT_EQ(a.fY, 22.0f);
    EXPECT_FLOAT_EQ(a.fZ, 33.0f);
}

// Verify operator-= modifies the vector in place
TEST(CVector, CompoundSubtract)
{
    CVector a(10.0f, 20.0f, 30.0f);
    a -= CVector(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(a.fX, 9.0f);
    EXPECT_FLOAT_EQ(a.fY, 18.0f);
    EXPECT_FLOAT_EQ(a.fZ, 27.0f);
}

///////////////////////////////////////////////////////////////
//
// Comparison / Validity
//
///////////////////////////////////////////////////////////////

// Verify operator== uses FLOAT_EPSILON tolerance and operator!= is its inverse
TEST(CVector, Equality)
{
    CVector a(1.0f, 2.0f, 3.0f);
    CVector b(1.0f, 2.0f, 3.0f);
    CVector c(1.0f, 2.0f, 3.1f);
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(a != c);
}

// Verify IsValid() rejects NaN and infinity
TEST(CVector, IsValid)
{
    CVector valid(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(valid.IsValid());

    CVector nan(std::numeric_limits<float>::quiet_NaN(), 0.0f, 0.0f);
    EXPECT_FALSE(nan.IsValid());

    CVector inf(0.0f, std::numeric_limits<float>::infinity(), 0.0f);
    EXPECT_FALSE(inf.IsValid());
}

// Verify operator[] accesses components by index (0=X, 1=Y, 2=Z)
TEST(CVector, IndexOperator)
{
    CVector v(10.0f, 20.0f, 30.0f);
    EXPECT_FLOAT_EQ(v[0], 10.0f);
    EXPECT_FLOAT_EQ(v[1], 20.0f);
    EXPECT_FLOAT_EQ(v[2], 30.0f);
}

///////////////////////////////////////////////////////////////
//
// Geometry helpers
//
///////////////////////////////////////////////////////////////

// Verify GetOtherAxis() returns a perpendicular unit vector
TEST(CVector, GetOtherAxis)
{
    CVector v(1.0f, 0.0f, 0.0f);
    CVector perp = v.GetOtherAxis();
    // Must be perpendicular (dot ~ 0) and unit length
    EXPECT_NEAR(v.DotProduct(&perp), 0.0f, kEpsilon);
    EXPECT_NEAR(perp.Length(), 1.0f, kEpsilon);
}
