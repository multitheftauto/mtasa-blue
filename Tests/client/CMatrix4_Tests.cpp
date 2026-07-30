/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CMatrix4_Tests.cpp
 *  PURPOSE:     Google Test suite for CMatrix4 (4x4 matrix math)
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include <CMatrix4.h>

static constexpr float kEpsilon = 0.001f;
static constexpr float kPi = 3.14159265f;

///////////////////////////////////////////////////////////////
//
// Construction & Identity
//
///////////////////////////////////////////////////////////////

// Verify default constructor produces an identity matrix (1s on diagonal, 0s elsewhere)
TEST(CMatrix4, DefaultConstructorIsIdentity)
{
    CMatrix4 m;
    EXPECT_FLOAT_EQ(m[0], 1.0f);
    EXPECT_FLOAT_EQ(m[5], 1.0f);
    EXPECT_FLOAT_EQ(m[10], 1.0f);
    EXPECT_FLOAT_EQ(m[15], 1.0f);
    // Off-diagonals should be zero
    EXPECT_FLOAT_EQ(m[1], 0.0f);
    EXPECT_FLOAT_EQ(m[4], 0.0f);
    EXPECT_FLOAT_EQ(m[12], 0.0f);
}

// Verify Identity() resets a modified matrix back to the identity matrix
TEST(CMatrix4, IdentityResetsMatrix)
{
    CMatrix4 m;
    m[1] = 99.0f;
    m.Identity();
    EXPECT_FLOAT_EQ(m[1], 0.0f);
    EXPECT_FLOAT_EQ(m[0], 1.0f);
}

///////////////////////////////////////////////////////////////
//
// Rotation builders
//
///////////////////////////////////////////////////////////////

// Verify 90-degree X rotation maps (0,1,0) to (0,0,1)
TEST(CMatrix4, RotXHalfPi)
{
    CMatrix4 m;
    m.rotX(kPi / 2.0f);
    // After 90-degree X rotation, (0,1,0) -> (0,0,1)
    CVector v(0.0f, 1.0f, 0.0f);
    CVector result = m.Multiply(v);
    EXPECT_NEAR(result.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 1.0f, kEpsilon);
}

// Verify 90-degree Y rotation maps (0,0,1) to (1,0,0)
TEST(CMatrix4, RotYHalfPi)
{
    CMatrix4 m;
    m.rotY(kPi / 2.0f);
    // After 90-degree Y rotation, (0,0,1) -> (1,0,0)
    CVector v(0.0f, 0.0f, 1.0f);
    CVector result = m.Multiply(v);
    EXPECT_NEAR(result.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 0.0f, kEpsilon);
}

// Verify 90-degree Z rotation maps (1,0,0) to (0,1,0)
TEST(CMatrix4, RotZHalfPi)
{
    CMatrix4 m;
    m.rotZ(kPi / 2.0f);
    // After 90-degree Z rotation, (1,0,0) -> (0,1,0)
    CVector v(1.0f, 0.0f, 0.0f);
    CVector result = m.Multiply(v);
    EXPECT_NEAR(result.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 0.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Translate
//
///////////////////////////////////////////////////////////////

// Verify Translate() transforms the origin to the given offset
TEST(CMatrix4, TranslateAddsOffset)
{
    CMatrix4 m;
    CVector  offset(10.0f, 20.0f, 30.0f);
    m.Translate(offset);
    // Identity + translation should transform origin to offset
    CVector origin(0.0f, 0.0f, 0.0f);
    CVector result = m.Multiply(origin);
    EXPECT_NEAR(result.fX, 10.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 20.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 30.0f, kEpsilon);
}

// Verify two successive Translate() calls accumulate the offsets
TEST(CMatrix4, TranslateAccumulates)
{
    CMatrix4 m;
    CVector  a(1.0f, 2.0f, 3.0f);
    CVector  b(4.0f, 5.0f, 6.0f);
    m.Translate(a);
    m.Translate(b);
    CVector result = m.Multiply(CVector(0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(result.fX, 5.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 7.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 9.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Multiply
//
///////////////////////////////////////////////////////////////

// Verify identity matrix leaves a vector unchanged after Multiply
TEST(CMatrix4, MultiplyIdentityPreservesVector)
{
    CMatrix4 m;
    CVector  v(5.0f, 10.0f, 15.0f);
    CVector  result = m.Multiply(v);
    EXPECT_NEAR(result.fX, 5.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 10.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 15.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Matrix multiplication (operator*)
//
///////////////////////////////////////////////////////////////

// Verify multiplying two identity matrices yields identity
TEST(CMatrix4, MultiplyTwoIdentitiesIsIdentity)
{
    CMatrix4 a;
    CMatrix4 b;
    CMatrix4 c = a * b;
    EXPECT_FLOAT_EQ(c[0], 1.0f);
    EXPECT_FLOAT_EQ(c[5], 1.0f);
    EXPECT_FLOAT_EQ(c[10], 1.0f);
    EXPECT_FLOAT_EQ(c[15], 1.0f);
    EXPECT_FLOAT_EQ(c[1], 0.0f);
}

// Verify operator* correctly chains a rotation then a translation
TEST(CMatrix4, MultiplyRotXThenTranslate)
{
    // Rotate 90 degrees around X, then translate by (0, 0, 5).
    // The combined matrix should rotate first, then translate.
    CMatrix4 rot;
    rot.rotX(kPi / 2.0f);

    CMatrix4 trans;
    CVector  offset(0.0f, 0.0f, 5.0f);
    trans.Translate(offset);

    // trans * rot means: first apply rot, then trans
    CMatrix4 combined = trans * rot;

    CVector v(0.0f, 1.0f, 0.0f);
    CVector result = combined.Multiply(v);
    // rotX(90) turns (0,1,0)->(0,0,1), then translate adds (0,0,5) -> (0,0,6)
    EXPECT_NEAR(result.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 6.0f, kEpsilon);
}

// Verify subscript operator allows reading and writing individual elements
TEST(CMatrix4, SubscriptOperator)
{
    CMatrix4 m;
    m[3] = 42.0f;
    EXPECT_FLOAT_EQ(m[3], 42.0f);
}
