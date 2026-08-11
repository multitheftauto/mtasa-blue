/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CMatrix_Tests.cpp
 *  PURPOSE:     Google Test suite for CMatrix (4x3 GTA matrix)
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include <CMatrix.h>

static constexpr float kEpsilon = 0.001f;

///////////////////////////////////////////////////////////////
//
// Construction
//
///////////////////////////////////////////////////////////////

// Verify default constructor produces the identity matrix (diagonal 1s, rest 0s)
TEST(CMatrix, DefaultConstructorIsIdentity)
{
    CMatrix m;
    EXPECT_FLOAT_EQ(m.vRight.fX, 1.0f);
    EXPECT_FLOAT_EQ(m.vRight.fY, 0.0f);
    EXPECT_FLOAT_EQ(m.vRight.fZ, 0.0f);
    EXPECT_FLOAT_EQ(m.vFront.fX, 0.0f);
    EXPECT_FLOAT_EQ(m.vFront.fY, 1.0f);
    EXPECT_FLOAT_EQ(m.vFront.fZ, 0.0f);
    EXPECT_FLOAT_EQ(m.vUp.fX, 0.0f);
    EXPECT_FLOAT_EQ(m.vUp.fY, 0.0f);
    EXPECT_FLOAT_EQ(m.vUp.fZ, 1.0f);
    EXPECT_FLOAT_EQ(m.vPos.fX, 0.0f);
    EXPECT_FLOAT_EQ(m.vPos.fY, 0.0f);
    EXPECT_FLOAT_EQ(m.vPos.fZ, 0.0f);
}

// Verify position-only constructor sets vPos and leaves rotation as identity
TEST(CMatrix, ConstructWithPosition)
{
    CVector pos(10.0f, 20.0f, 30.0f);
    CMatrix m(pos);
    EXPECT_FLOAT_EQ(m.vPos.fX, 10.0f);
    EXPECT_FLOAT_EQ(m.vPos.fY, 20.0f);
    EXPECT_FLOAT_EQ(m.vPos.fZ, 30.0f);
    // Rotation part should still be identity
    EXPECT_FLOAT_EQ(m.vRight.fX, 1.0f);
    EXPECT_FLOAT_EQ(m.vFront.fY, 1.0f);
    EXPECT_FLOAT_EQ(m.vUp.fZ, 1.0f);
}

// Verify position+rotation constructor applies zero rotation without breaking identity
TEST(CMatrix, ConstructWithPositionAndRotation)
{
    CVector pos(5.0f, 0.0f, 0.0f);
    CVector rot(0.0f, 0.0f, 0.0f);
    CMatrix m(pos, rot);
    // Zero rotation should preserve identity rotation
    EXPECT_NEAR(m.vRight.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(m.vFront.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(m.vUp.fZ, 1.0f, kEpsilon);
    EXPECT_FLOAT_EQ(m.vPos.fX, 5.0f);
}

// Verify position+rotation+scale constructor sets the correct scale on each axis
TEST(CMatrix, ConstructWithPositionRotationScale)
{
    CVector pos(0.0f, 0.0f, 0.0f);
    CVector rot(0.0f, 0.0f, 0.0f);
    CVector scale(2.0f, 3.0f, 4.0f);
    CMatrix m(pos, rot, scale);
    CVector s = m.GetScale();
    EXPECT_NEAR(s.fX, 2.0f, kEpsilon);
    EXPECT_NEAR(s.fY, 3.0f, kEpsilon);
    EXPECT_NEAR(s.fZ, 4.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Arithmetic operators
//
///////////////////////////////////////////////////////////////

// Verify operator+ adds all four row vectors component-wise
TEST(CMatrix, Addition)
{
    CMatrix a(CVector(1.0f, 2.0f, 3.0f));
    CMatrix b(CVector(4.0f, 5.0f, 6.0f));
    CMatrix c = a + b;
    EXPECT_FLOAT_EQ(c.vPos.fX, 5.0f);
    EXPECT_FLOAT_EQ(c.vPos.fY, 7.0f);
    EXPECT_FLOAT_EQ(c.vPos.fZ, 9.0f);
    // vRight should be (1,0,0) + (1,0,0) = (2,0,0)
    EXPECT_FLOAT_EQ(c.vRight.fX, 2.0f);
}

// Verify operator- subtracts all four row vectors component-wise
TEST(CMatrix, Subtraction)
{
    CMatrix a(CVector(10.0f, 20.0f, 30.0f));
    CMatrix b(CVector(3.0f, 5.0f, 7.0f));
    CMatrix c = a - b;
    EXPECT_FLOAT_EQ(c.vPos.fX, 7.0f);
    EXPECT_FLOAT_EQ(c.vPos.fY, 15.0f);
    EXPECT_FLOAT_EQ(c.vPos.fZ, 23.0f);
}

// Verify multiplying a matrix by identity preserves position and rotation
TEST(CMatrix, MultiplyIdentity)
{
    // Multiplying by identity should return the original matrix
    CMatrix a(CVector(1.0f, 2.0f, 3.0f));
    CMatrix identity;
    CMatrix c = a * identity;
    EXPECT_NEAR(c.vPos.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(c.vPos.fY, 2.0f, kEpsilon);
    EXPECT_NEAR(c.vPos.fZ, 3.0f, kEpsilon);
    EXPECT_NEAR(c.vRight.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(c.vFront.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(c.vUp.fZ, 1.0f, kEpsilon);
}

// Verify matrix*vector applies rotation only (no translation) as per operator* semantics
TEST(CMatrix, MultiplyVector)
{
    // Identity * vector = same direction (no translation applied)
    CMatrix identity;
    CVector v(1.0f, 2.0f, 3.0f);
    CVector result = identity * v;
    EXPECT_NEAR(result.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 2.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 3.0f, kEpsilon);
}

// Verify operator/ by identity preserves the original matrix (uses Invert, not Inverse)
TEST(CMatrix, DivideByIdentityPreservesMatrix)
{
    // operator/ uses Invert() (transpose + negate position), which is only
    // a valid inverse for orthonormal matrices. Dividing by identity
    // should preserve the original matrix.
    CVector pos(1.0f, 2.0f, 3.0f);
    CVector rot(0.3f, 0.5f, 0.7f);
    CMatrix a(pos, rot);
    CMatrix identity;
    CMatrix result = a / identity;

    EXPECT_NEAR(result.vPos.fX, a.vPos.fX, kEpsilon);
    EXPECT_NEAR(result.vPos.fY, a.vPos.fY, kEpsilon);
    EXPECT_NEAR(result.vPos.fZ, a.vPos.fZ, kEpsilon);
    EXPECT_NEAR(result.vRight.fX, a.vRight.fX, kEpsilon);
    EXPECT_NEAR(result.vFront.fY, a.vFront.fY, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Inverse
//
///////////////////////////////////////////////////////////////

// Verify Inverse() of identity is still identity
TEST(CMatrix, InverseOfIdentityIsIdentity)
{
    CMatrix identity;
    CMatrix inv = identity.Inverse();
    EXPECT_NEAR(inv.vRight.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(inv.vFront.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(inv.vUp.fZ, 1.0f, kEpsilon);
    EXPECT_NEAR(inv.vPos.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(inv.vPos.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(inv.vPos.fZ, 0.0f, kEpsilon);
}

// Verify M * M^-1 produces the identity matrix (inverse roundtrip)
TEST(CMatrix, InverseRoundtrip)
{
    // M * M^-1 should produce identity
    CVector pos(5.0f, 10.0f, 15.0f);
    CVector rot(0.3f, 0.5f, 0.7f);
    CMatrix m(pos, rot);
    CMatrix inv = m.Inverse();
    CMatrix result = m * inv;
    EXPECT_NEAR(result.vRight.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(result.vRight.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vRight.fZ, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vFront.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vFront.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(result.vFront.fZ, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vUp.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vUp.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vUp.fZ, 1.0f, kEpsilon);
    EXPECT_NEAR(result.vPos.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vPos.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.vPos.fZ, 0.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Rotation get/set roundtrip
//
///////////////////////////////////////////////////////////////

// Verify SetRotation then GetRotation returns the same Euler angles
TEST(CMatrix, SetRotationGetRotationRoundtrip)
{
    // A key property: SetRotation followed by GetRotation should return the same angles
    CVector rot(0.4f, 0.6f, -0.8f);
    CMatrix m;
    m.SetRotation(rot);
    CVector result = m.GetRotation();
    EXPECT_NEAR(result.fX, rot.fX, kEpsilon);
    EXPECT_NEAR(result.fY, rot.fY, kEpsilon);
    EXPECT_NEAR(result.fZ, rot.fZ, kEpsilon);
}

// Verify SetRotation does not alter the matrix scale
TEST(CMatrix, SetRotationPreservesScale)
{
    CVector scale(2.0f, 3.0f, 4.0f);
    CMatrix m(CVector(), CVector(), scale);
    CVector scaleBeforeRot = m.GetScale();
    m.SetRotation(CVector(0.5f, 0.5f, 0.5f));
    CVector scaleAfterRot = m.GetScale();
    EXPECT_NEAR(scaleBeforeRot.fX, scaleAfterRot.fX, kEpsilon);
    EXPECT_NEAR(scaleBeforeRot.fY, scaleAfterRot.fY, kEpsilon);
    EXPECT_NEAR(scaleBeforeRot.fZ, scaleAfterRot.fZ, kEpsilon);
}

// Verify SetRotation(0,0,0) restores the identity rotation part
TEST(CMatrix, ZeroRotationProducesIdentityRotation)
{
    CMatrix m;
    m.SetRotation(CVector(0.0f, 0.0f, 0.0f));
    EXPECT_NEAR(m.vRight.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(m.vFront.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(m.vUp.fZ, 1.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Position get/set
//
///////////////////////////////////////////////////////////////

// Verify SetPosition/GetPosition roundtrip stores and retrieves the position
TEST(CMatrix, SetPositionGetPosition)
{
    CMatrix m;
    CVector pos(100.0f, 200.0f, 300.0f);
    m.SetPosition(pos);
    const CVector& result = m.GetPosition();
    EXPECT_FLOAT_EQ(result.fX, 100.0f);
    EXPECT_FLOAT_EQ(result.fY, 200.0f);
    EXPECT_FLOAT_EQ(result.fZ, 300.0f);
}

///////////////////////////////////////////////////////////////
//
// Scale get/set
//
///////////////////////////////////////////////////////////////

// Verify default identity matrix has unit scale on all axes
TEST(CMatrix, DefaultScaleIsOne)
{
    CMatrix m;
    CVector s = m.GetScale();
    EXPECT_NEAR(s.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(s.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(s.fZ, 1.0f, kEpsilon);
}

// Verify SetScale/GetScale roundtrip stores and retrieves the scale
TEST(CMatrix, SetScaleGetScale)
{
    CMatrix m;
    m.SetScale(CVector(5.0f, 6.0f, 7.0f));
    CVector s = m.GetScale();
    EXPECT_NEAR(s.fX, 5.0f, kEpsilon);
    EXPECT_NEAR(s.fY, 6.0f, kEpsilon);
    EXPECT_NEAR(s.fZ, 7.0f, kEpsilon);
}

// Verify SetScale does not alter the rotation angles
TEST(CMatrix, SetScalePreservesRotation)
{
    CVector rot(0.3f, 0.5f, 0.7f);
    CMatrix m(CVector(), rot);
    CVector rotBefore = m.GetRotation();
    m.SetScale(CVector(2.0f, 2.0f, 2.0f));
    CVector rotAfter = m.GetRotation();
    EXPECT_NEAR(rotBefore.fX, rotAfter.fX, kEpsilon);
    EXPECT_NEAR(rotBefore.fY, rotAfter.fY, kEpsilon);
    EXPECT_NEAR(rotBefore.fZ, rotAfter.fZ, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// TransformVector / TransformVectorByRotation
//
///////////////////////////////////////////////////////////////

// Verify TransformVector applies both rotation and translation to the input vector
TEST(CMatrix, TransformVectorIncludesPosition)
{
    CMatrix m(CVector(10.0f, 20.0f, 30.0f));
    CVector v(1.0f, 0.0f, 0.0f);
    CVector result = m.TransformVector(v);
    // Identity rotation + position offset
    EXPECT_NEAR(result.fX, 11.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 20.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 30.0f, kEpsilon);
}

// Verify TransformVectorByRotation applies rotation but ignores translation
TEST(CMatrix, TransformVectorByRotationExcludesPosition)
{
    CMatrix m(CVector(10.0f, 20.0f, 30.0f));
    CVector v(1.0f, 0.0f, 0.0f);
    CVector result = m.TransformVectorByRotation(v);
    // Identity rotation, no position applied
    EXPECT_NEAR(result.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 0.0f, kEpsilon);
}

// Verify TransformVectorByRotation correctly rotates (1,0,0) by 90 degrees around Z
TEST(CMatrix, TransformVectorByRotationAppliesRotation)
{
    // Rotate 90 degrees around Z axis
    float   halfPi = 1.5707963f;
    CMatrix m(CVector(), CVector(0.0f, 0.0f, halfPi));
    CVector v(1.0f, 0.0f, 0.0f);
    CVector result = m.TransformVectorByRotation(v);
    // (1,0,0) rotated 90 degrees around Z should give roughly (0,1,0)
    EXPECT_NEAR(result.fX, 0.0f, kEpsilon);
    EXPECT_NEAR(result.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(result.fZ, 0.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// OrthoNormalize
//
///////////////////////////////////////////////////////////////

// Verify OrthoNormalize produces unit-length axes from scaled-up axes
TEST(CMatrix, OrthoNormalizeProducesUnitAxes)
{
    CMatrix m;
    // Scale up the axes to make them non-unit
    m.vRight = CVector(3.0f, 0.0f, 0.0f);
    m.vFront = CVector(0.0f, 5.0f, 0.0f);
    m.vUp = CVector(0.0f, 0.0f, 7.0f);
    m.OrthoNormalize(CMatrix::AXIS_RIGHT, CMatrix::AXIS_FRONT);
    EXPECT_NEAR(m.vRight.Length(), 1.0f, kEpsilon);
    EXPECT_NEAR(m.vFront.Length(), 1.0f, kEpsilon);
    EXPECT_NEAR(m.vUp.Length(), 1.0f, kEpsilon);
}

// Verify OrthoNormalize keeps the primary axis direction unchanged
TEST(CMatrix, OrthoNormalizePreservesPrimaryAxisDirection)
{
    CMatrix m;
    m.vRight = CVector(2.0f, 0.0f, 0.0f);
    m.vFront = CVector(0.1f, 3.0f, 0.0f);  // slightly off perpendicular
    m.OrthoNormalize(CMatrix::AXIS_RIGHT, CMatrix::AXIS_FRONT);
    // Right should still point along +X
    EXPECT_NEAR(m.vRight.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(m.vRight.fY, 0.0f, kEpsilon);
    EXPECT_NEAR(m.vRight.fZ, 0.0f, kEpsilon);
}

///////////////////////////////////////////////////////////////
//
// Clone / Invert / GetBuffer / To4x4Array
//
///////////////////////////////////////////////////////////////

// Verify Clone() produces an independent copy with identical values
TEST(CMatrix, CloneProducesCopy)
{
    CMatrix m(CVector(1.0f, 2.0f, 3.0f), CVector(0.5f, 0.5f, 0.5f));
    CMatrix c = m.Clone();
    EXPECT_FLOAT_EQ(m.vPos.fX, c.vPos.fX);
    EXPECT_FLOAT_EQ(m.vPos.fY, c.vPos.fY);
    EXPECT_FLOAT_EQ(m.vPos.fZ, c.vPos.fZ);
    EXPECT_FLOAT_EQ(m.vRight.fX, c.vRight.fX);
    EXPECT_FLOAT_EQ(m.vFront.fY, c.vFront.fY);
    EXPECT_FLOAT_EQ(m.vUp.fZ, c.vUp.fZ);
}

// Verify Invert() transposes the 3x3 rotation and negates position
TEST(CMatrix, InvertTransposesRotationAndNegatesPosition)
{
    CMatrix m;
    m.vPos = CVector(5.0f, 10.0f, 15.0f);
    m.Invert();
    EXPECT_FLOAT_EQ(m.vPos.fX, -5.0f);
    EXPECT_FLOAT_EQ(m.vPos.fY, -10.0f);
    EXPECT_FLOAT_EQ(m.vPos.fZ, -15.0f);
    // Identity rotation transposed is still identity
    EXPECT_FLOAT_EQ(m.vRight.fX, 1.0f);
    EXPECT_FLOAT_EQ(m.vFront.fY, 1.0f);
    EXPECT_FLOAT_EQ(m.vUp.fZ, 1.0f);
}

// Verify GetBuffer() writes position to elements [12..14] and 1.0 to [15]
TEST(CMatrix, GetBufferLayout)
{
    CMatrix m(CVector(10.0f, 20.0f, 30.0f));
    float   buf[16];
    m.GetBuffer(buf);
    // Position should be in elements 12-14
    EXPECT_FLOAT_EQ(buf[12], 10.0f);
    EXPECT_FLOAT_EQ(buf[13], 20.0f);
    EXPECT_FLOAT_EQ(buf[14], 30.0f);
    EXPECT_FLOAT_EQ(buf[15], 1.0f);
}

// Verify To4x4Array() places position in row 3 with w=1.0
TEST(CMatrix, To4x4ArrayLayout)
{
    CMatrix m(CVector(10.0f, 20.0f, 30.0f));
    auto    arr = m.To4x4Array();
    // Position should be row 3 (index [3])
    EXPECT_FLOAT_EQ(arr[3][0], 10.0f);
    EXPECT_FLOAT_EQ(arr[3][1], 20.0f);
    EXPECT_FLOAT_EQ(arr[3][2], 30.0f);
    EXPECT_FLOAT_EQ(arr[3][3], 1.0f);
}

// Verify GetAxis() returns the correct axis vector by enum index
TEST(CMatrix, GetAxis)
{
    CMatrix m;
    EXPECT_FLOAT_EQ(m.GetAxis(CMatrix::AXIS_RIGHT).fX, 1.0f);
    EXPECT_FLOAT_EQ(m.GetAxis(CMatrix::AXIS_FRONT).fY, 1.0f);
    EXPECT_FLOAT_EQ(m.GetAxis(CMatrix::AXIS_UP).fZ, 1.0f);
}

// Verify GetRotationMatrix() strips scale to unit and zeros out position
TEST(CMatrix, GetRotationMatrix)
{
    CVector scale(2.0f, 3.0f, 4.0f);
    CMatrix m(CVector(5.0f, 6.0f, 7.0f), CVector(0.3f, 0.5f, 0.7f), scale);
    CMatrix rotMat = m.GetRotationMatrix();
    // Rotation matrix should have unit scale
    CVector s = rotMat.GetScale();
    EXPECT_NEAR(s.fX, 1.0f, kEpsilon);
    EXPECT_NEAR(s.fY, 1.0f, kEpsilon);
    EXPECT_NEAR(s.fZ, 1.0f, kEpsilon);
    // Position should be zeroed out
    EXPECT_FLOAT_EQ(rotMat.vPos.fX, 0.0f);
    EXPECT_FLOAT_EQ(rotMat.vPos.fY, 0.0f);
    EXPECT_FLOAT_EQ(rotMat.vPos.fZ, 0.0f);
}
