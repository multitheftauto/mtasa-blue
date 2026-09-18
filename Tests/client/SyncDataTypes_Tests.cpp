/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SyncDataTypes_Tests.cpp
 *  PURPOSE:     Round-trip tests for primitive sync types (SFloatSync,
 *               SIntegerSync) and health/armor syncs (SFloatAsBitsSync
 *               derivatives)
 *
 *  These structures are the building blocks of higher-level sync packets.
 *  SFloatSync quantizes a float into an integer+fractional bit split;
 *  SFloatAsBitsSync maps a float range [min,max] into N bits.
 *  Both have inherent precision loss - tolerances are derived from the
 *  theoretical step size of each encoding.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include "MockBitStream.h"
#include <net/SyncStructures.h>

// ============================================================================
// SFloatSync round-trips
// ============================================================================

// SFloatSync<intBits, fracBits> stores a signed integer part in intBits
// and a fractional part in fracBits. The total wire size is intBits+fracBits.
// Precision = 1 / (1 << fracBits).

// Zero should encode and decode exactly.
TEST(SFloatSync, RoundTrip_14_10_Zero)
{
    MockBitStream      bs;
    SFloatSync<14, 10> sync;
    sync.data.fValue = 0.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SFloatSync<14, 10> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(0.0f, out.data.fValue, 1.0f / (1 << 10));
}

// A positive value in the representable range should round-trip within
// the fractional precision of 1/1024.
TEST(SFloatSync, RoundTrip_14_10_Positive)
{
    MockBitStream      bs;
    SFloatSync<14, 10> sync;
    sync.data.fValue = 1234.5f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SFloatSync<14, 10> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1234.5f, out.data.fValue, 1.0f / (1 << 10) + 1e-3f);
}

// Negative values are encoded via a sign bit in the integer portion.
TEST(SFloatSync, RoundTrip_14_10_Negative)
{
    MockBitStream      bs;
    SFloatSync<14, 10> sync;
    sync.data.fValue = -4000.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SFloatSync<14, 10> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(-4000.0f, out.data.fValue, 1.0f / (1 << 10) + 1e-3f);
}

// Values beyond the representable range ((1 << 13) - 1 = 8191) are clamped.
// This verifies the clamp behavior rather than silent overflow.
TEST(SFloatSync, RoundTrip_14_10_ClampMax)
{
    MockBitStream      bs;
    SFloatSync<14, 10> sync;
    sync.data.fValue = 99999.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SFloatSync<14, 10> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(8191.0f, out.data.fValue, 1.0f / (1 << 10) + 1e-3f);
}

// The wire format should use exactly intBits + fracBits = 24 bits.
TEST(SFloatSync, BitCount)
{
    MockBitStream      bs;
    SFloatSync<14, 10> sync;
    sync.data.fValue = 42.0f;
    sync.Write(bs);
    EXPECT_EQ(14 + 10, bs.GetNumberOfBitsUsed());
}

// ============================================================================
// SIntegerSync round-trips
// ============================================================================

// SIntegerSync<T, bits> writes exactly `bits` low-order bits of a value.

// 4-bit unsigned: range [0, 15].
TEST(SIntegerSync, RoundTrip_4Bit)
{
    MockBitStream                 bs;
    SIntegerSync<unsigned int, 4> sync;
    sync.data.value = 13;
    sync.Write(bs);
    EXPECT_EQ(4, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SIntegerSync<unsigned int, 4> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(13u, static_cast<unsigned int>(out.data.value));
}

// 6-bit unsigned char: range [0, 63].
TEST(SIntegerSync, RoundTrip_6Bit)
{
    MockBitStream                  bs;
    SIntegerSync<unsigned char, 6> sync;
    sync.data.value = 44;
    sync.Write(bs);
    EXPECT_EQ(6, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SIntegerSync<unsigned char, 6> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(44, static_cast<int>(out.data.value));
}

// ============================================================================
// Health / Armor syncs (SFloatAsBitsSync derivatives)
// ============================================================================

// SFloatAsBitsSync maps a float range [min, max] into N bits.
// Step size = (max - min) / ((1 << N) - 1).
// bPreserveGreaterThanMin: if true, any value > min reads back as > min.

// Player health: 8 bits over [0, 255]. Step = 1.0.
TEST(SPlayerHealthSync, RoundTrip_Zero)
{
    MockBitStream     bs;
    SPlayerHealthSync sync;
    sync.data.fValue = 0.0f;
    sync.Write(bs);
    EXPECT_EQ(8, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SPlayerHealthSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(0.0f, out.data.fValue, 1.1f);
}

// 200 HP is in-range and should round-trip with step-size tolerance.
TEST(SPlayerHealthSync, RoundTrip_MaxHealth)
{
    MockBitStream     bs;
    SPlayerHealthSync sync;
    sync.data.fValue = 200.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SPlayerHealthSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(200.0f, out.data.fValue, 1.1f);
}

// bPreserveGreaterThanMin=true ensures that any health > 0 reads back as > 0.
// This is important for preventing "dead" false positives during sync.
TEST(SPlayerHealthSync, RoundTrip_SmallPositive)
{
    MockBitStream     bs;
    SPlayerHealthSync sync;
    sync.data.fValue = 0.1f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SPlayerHealthSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_GT(out.data.fValue, 0.0f) << "PreserveGreaterThanMin should keep small values above zero";
}

// Player armor: 8 bits over [0, 127.5]. Step = 0.5.
TEST(SPlayerArmorSync, RoundTrip_FullArmor)
{
    MockBitStream    bs;
    SPlayerArmorSync sync;
    sync.data.fValue = 100.0f;
    sync.Write(bs);
    EXPECT_EQ(8, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SPlayerArmorSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(100.0f, out.data.fValue, 0.6f);
}

// Vehicle health: 12 bits over [0, 2047.5]. Step = 0.5.
TEST(SVehicleHealthSync, RoundTrip_MidRange)
{
    MockBitStream      bs;
    SVehicleHealthSync sync;
    sync.data.fValue = 1000.0f;
    sync.Write(bs);
    EXPECT_EQ(12, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SVehicleHealthSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1000.0f, out.data.fValue, 0.6f);
}

// Low-precision vehicle health: 8 bits over [0, 2040]. Step = 8.
// Used when bandwidth savings outweigh precision.
TEST(SLowPrecisionVehicleHealthSync, RoundTrip)
{
    MockBitStream                  bs;
    SLowPrecisionVehicleHealthSync sync;
    sync.data.fValue = 1000.0f;
    sync.Write(bs);
    EXPECT_EQ(8, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SLowPrecisionVehicleHealthSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1000.0f, out.data.fValue, 9.0f);
}

// Object health: 11 bits over [0, 1023.5]. Step = 0.5.
TEST(SObjectHealthSync, RoundTrip)
{
    MockBitStream     bs;
    SObjectHealthSync sync;
    sync.data.fValue = 500.0f;
    sync.Write(bs);
    EXPECT_EQ(11, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SObjectHealthSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(500.0f, out.data.fValue, 0.6f);
}
