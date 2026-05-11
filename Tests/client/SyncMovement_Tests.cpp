/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SyncMovement_Tests.cpp
 *  PURPOSE:     Round-trip tests for position, rotation, and velocity
 *               sync structures
 *
 *  Position syncs have two modes: float (raw 32-bit) and quantized
 *  (SFloatSync<14,10> for X/Y, raw float for Z). Rotation syncs
 *  quantize angles into 16-bit ranges. Velocity uses a flag bit to skip
 *  encoding when the vector is zero, and NormVector encoding for direction.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include "MockBitStream.h"
#include <net/SyncStructures.h>

// ============================================================================
// Position syncs
// ============================================================================

// Float mode writes all three components as raw 32-bit floats - no precision loss.
TEST(SPositionSync, RoundTrip_Floats)
{
    MockBitStream bs;
    SPositionSync sync(true);
    sync.data.vecPosition.fX = 100.5f;
    sync.data.vecPosition.fY = -200.25f;
    sync.data.vecPosition.fZ = 50.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SPositionSync out(true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(100.5f, out.data.vecPosition.fX);
    EXPECT_FLOAT_EQ(-200.25f, out.data.vecPosition.fY);
    EXPECT_FLOAT_EQ(50.0f, out.data.vecPosition.fZ);
}

// Quantized mode: X and Y use SFloatSync<14,10> (precision ~0.001),
// Z is still a raw float (vertical position needs full precision for
// building interiors, etc).
TEST(SPositionSync, RoundTrip_Quantized)
{
    MockBitStream bs;
    SPositionSync sync(false);
    sync.data.vecPosition.fX = 500.0f;
    sync.data.vecPosition.fY = -1234.0f;
    sync.data.vecPosition.fZ = 42.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SPositionSync out(false);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(500.0f, out.data.vecPosition.fX, 0.01f);
    EXPECT_NEAR(-1234.0f, out.data.vecPosition.fY, 0.01f);
    EXPECT_FLOAT_EQ(42.0f, out.data.vecPosition.fZ);
}

// 2D position (float): only X and Y, used for map blips and radar positions.
TEST(SPosition2DSync, RoundTrip_Floats)
{
    MockBitStream   bs;
    SPosition2DSync sync(true);
    sync.data.vecPosition.fX = 300.0f;
    sync.data.vecPosition.fY = -150.5f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SPosition2DSync out(true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(300.0f, out.data.vecPosition.fX);
    EXPECT_FLOAT_EQ(-150.5f, out.data.vecPosition.fY);
}

// 2D position (quantized): uses SFloatSync<14,10> for both components.
TEST(SPosition2DSync, RoundTrip_Quantized)
{
    MockBitStream   bs;
    SPosition2DSync sync(false);
    sync.data.vecPosition.fX = 2000.0f;
    sync.data.vecPosition.fY = -3000.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SPosition2DSync out(false);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(2000.0f, out.data.vecPosition.fX, 0.01f);
    EXPECT_NEAR(-3000.0f, out.data.vecPosition.fY, 0.01f);
}

// Low-precision position: X/Y use 16-bit over [-8192, 8192] (step ~0.25),
// Z uses an 11-bit integer offset by -110 (step = 1). Used for lightweight
// sync where exact position is less critical.
TEST(SLowPrecisionPositionSync, RoundTrip)
{
    MockBitStream             bs;
    SLowPrecisionPositionSync sync;
    sync.data.vecPosition.fX = 1000.0f;
    sync.data.vecPosition.fY = -2000.0f;
    sync.data.vecPosition.fZ = 50.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SLowPrecisionPositionSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1000.0f, out.data.vecPosition.fX, 0.3f);
    EXPECT_NEAR(-2000.0f, out.data.vecPosition.fY, 0.3f);
    EXPECT_NEAR(50.0f, out.data.vecPosition.fZ, 1.1f);
}

// ============================================================================
// Rotation syncs
// ============================================================================

// Degrees (quantized): each axis uses 16-bit over [0, 360). Step ~0.0055 degrees.
TEST(SRotationDegreesSync, RoundTrip_Quantized)
{
    MockBitStream        bs;
    SRotationDegreesSync sync(false);
    sync.data.vecRotation.fX = 90.0f;
    sync.data.vecRotation.fY = 180.0f;
    sync.data.vecRotation.fZ = 270.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SRotationDegreesSync out(false);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(90.0f, out.data.vecRotation.fX, 0.01f);
    EXPECT_NEAR(180.0f, out.data.vecRotation.fY, 0.01f);
    EXPECT_NEAR(270.0f, out.data.vecRotation.fZ, 0.01f);
}

// Negative rotation: SRotationDegreesSync casts to unsigned short, so
// negative inputs wrap around. For example, -10 degrees → unsigned short
// wraps to a large value, which reads back as ~360 + (-10 * 65536/360)
// mapped to [0, 360). This is the expected behavior for the sync struct.
TEST(SRotationDegreesSync, RoundTrip_NegativeInput)
{
    MockBitStream        bs;
    SRotationDegreesSync sync(false);
    sync.data.vecRotation.fX = -10.0f;
    sync.data.vecRotation.fY = -90.0f;
    sync.data.vecRotation.fZ = -180.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SRotationDegreesSync out(false);
    EXPECT_TRUE(out.Read(bs));
    // Negative values wrap into [0, 360) range due to unsigned short cast.
    // -10° → cast to unsigned short → wraps → reads back as ~350°
    EXPECT_NEAR(350.0f, out.data.vecRotation.fX, 0.01f);
    EXPECT_NEAR(270.0f, out.data.vecRotation.fY, 0.01f);
    EXPECT_NEAR(180.0f, out.data.vecRotation.fZ, 0.01f);
}

// Degrees (float): raw 32-bit floats, no precision loss.
TEST(SRotationDegreesSync, RoundTrip_Floats)
{
    MockBitStream        bs;
    SRotationDegreesSync sync(true);
    sync.data.vecRotation.fX = 45.5f;
    sync.data.vecRotation.fY = 123.456f;
    sync.data.vecRotation.fZ = 359.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SRotationDegreesSync out(true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(45.5f, out.data.vecRotation.fX);
    EXPECT_FLOAT_EQ(123.456f, out.data.vecRotation.fY);
    EXPECT_FLOAT_EQ(359.0f, out.data.vecRotation.fZ);
}

// Radians (quantized): each axis uses 16-bit over [0, 2*PI). Step ~0.0001 radians.
TEST(SRotationRadiansSync, RoundTrip_Quantized)
{
    MockBitStream        bs;
    SRotationRadiansSync sync(false);
    sync.data.vecRotation.fX = 1.0f;
    sync.data.vecRotation.fY = 3.0f;
    sync.data.vecRotation.fZ = 5.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SRotationRadiansSync out(false);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1.0f, out.data.vecRotation.fX, 0.001f);
    EXPECT_NEAR(3.0f, out.data.vecRotation.fY, 0.001f);
    EXPECT_NEAR(5.0f, out.data.vecRotation.fZ, 0.001f);
}

// ============================================================================
// Velocity sync
// ============================================================================

// Zero velocity writes only 1 bit (the "has velocity" flag = false).
// This is a bandwidth optimization - stationary entities are common.
TEST(SVelocitySync, RoundTrip_ZeroVelocity)
{
    MockBitStream bs;
    SVelocitySync sync;
    sync.data.vecVelocity.fX = 0.0f;
    sync.data.vecVelocity.fY = 0.0f;
    sync.data.vecVelocity.fZ = 0.0f;
    sync.Write(bs);
    EXPECT_EQ(1, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SVelocitySync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(0.0f, out.data.vecVelocity.fX);
    EXPECT_FLOAT_EQ(0.0f, out.data.vecVelocity.fY);
    EXPECT_FLOAT_EQ(0.0f, out.data.vecVelocity.fZ);
}

// Non-zero velocity: direction is NormVector-encoded (16-bit per component),
// magnitude is a raw float. The reconstructed vector = direction * magnitude.
TEST(SVelocitySync, RoundTrip_NonZero)
{
    MockBitStream bs;
    SVelocitySync sync;
    sync.data.vecVelocity.fX = 10.0f;
    sync.data.vecVelocity.fY = 0.0f;
    sync.data.vecVelocity.fZ = 0.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVelocitySync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(10.0f, out.data.vecVelocity.fX, 0.01f);
    EXPECT_NEAR(0.0f, out.data.vecVelocity.fY, 0.01f);
    EXPECT_NEAR(0.0f, out.data.vecVelocity.fZ, 0.01f);
}

// Diagonal velocity: tests that all three components of the NormVector
// survive encoding with acceptable precision.
TEST(SVelocitySync, RoundTrip_DiagonalVelocity)
{
    MockBitStream bs;
    SVelocitySync sync;
    sync.data.vecVelocity.fX = 5.0f;
    sync.data.vecVelocity.fY = 5.0f;
    sync.data.vecVelocity.fZ = 5.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVelocitySync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(5.0f, out.data.vecVelocity.fX, 0.05f);
    EXPECT_NEAR(5.0f, out.data.vecVelocity.fY, 0.05f);
    EXPECT_NEAR(5.0f, out.data.vecVelocity.fZ, 0.05f);
}
