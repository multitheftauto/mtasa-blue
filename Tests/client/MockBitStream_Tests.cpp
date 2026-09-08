/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/MockBitStream_Tests.cpp
 *  PURPOSE:     Self-tests for the MockBitStream test double
 *
 *  These tests verify that MockBitStream itself behaves correctly before we
 *  rely on it to test SyncStructures. If any of these fail, all other sync
 *  structure tests are suspect.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include "MockBitStream.h"
#include <net/SyncStructures.h>

// Verify that a simple unsigned char survives a write→read round-trip.
TEST(MockBitStream, ScalarRoundTrip_UnsignedChar)
{
    MockBitStream bs;
    unsigned char input = 0xAB;
    bs.Write(input);
    bs.ResetReadPointer();
    unsigned char output = 0;
    EXPECT_TRUE(bs.Read(output));
    EXPECT_EQ(input, output);
}

// Verify that a 32-bit float survives a write→read round-trip bit-exactly.
TEST(MockBitStream, ScalarRoundTrip_Float)
{
    MockBitStream bs;
    float         input = -123.456f;
    bs.Write(input);
    bs.ResetReadPointer();
    float output = 0.0f;
    EXPECT_TRUE(bs.Read(output));
    EXPECT_FLOAT_EQ(input, output);
}

// Verify sub-byte bit-level writes: write 5 bits (value 21 = 0b10101),
// confirm the stream uses exactly 5 bits, and read them back.
TEST(MockBitStream, BitLevelReadWrite)
{
    MockBitStream bs;
    unsigned char val = 21;
    bs.WriteBits(reinterpret_cast<const char*>(&val), 5);
    EXPECT_EQ(5, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    unsigned char out = 0;
    EXPECT_TRUE(bs.ReadBits(reinterpret_cast<char*>(&out), 5));
    EXPECT_EQ(21, out);
}

// Verify individual bit read/write: write true, false, true and read them back
// in the same order.
TEST(MockBitStream, SingleBitReadWrite)
{
    MockBitStream bs;
    bs.WriteBit(true);
    bs.WriteBit(false);
    bs.WriteBit(true);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    EXPECT_TRUE(bs.ReadBit());
    EXPECT_FALSE(bs.ReadBit());
    EXPECT_TRUE(bs.ReadBit());
}

// Verify compressed write/read for a small unsigned short (value 42).
// Small values should compress to fewer bits since the high byte is zero.
TEST(MockBitStream, CompressedRoundTrip_UnsignedShort)
{
    MockBitStream  bs;
    unsigned short input = 42;
    bs.WriteCompressed(input);
    bs.ResetReadPointer();
    unsigned short output = 0;
    EXPECT_TRUE(bs.ReadCompressed(output));
    EXPECT_EQ(input, output);
}

// Verify compressed write/read for a large unsigned short (value 60000).
// Large values need the full 16 bits plus the compression flag bit.
TEST(MockBitStream, CompressedRoundTrip_UnsignedShort_Large)
{
    MockBitStream  bs;
    unsigned short input = 60000;
    bs.WriteCompressed(input);
    bs.ResetReadPointer();
    unsigned short output = 0;
    EXPECT_TRUE(bs.ReadCompressed(output));
    EXPECT_EQ(input, output);
}

// Verify compressed write/read for unsigned char. Single-byte types write
// all 8 bits without a compression flag.
TEST(MockBitStream, CompressedRoundTrip_UnsignedChar)
{
    MockBitStream bs;
    unsigned char input = 200;
    bs.WriteCompressed(input);
    bs.ResetReadPointer();
    unsigned char output = 0;
    EXPECT_TRUE(bs.ReadCompressed(output));
    EXPECT_EQ(input, output);
}

// Verify NormVector round-trip with an axis-aligned unit vector (1,0,0).
// Each component is encoded as a 16-bit signed integer scaled by 32767.
TEST(MockBitStream, NormVectorRoundTrip)
{
    MockBitStream bs;
    float         x = 1.0f, y = 0.0f, z = 0.0f;
    bs.WriteNormVector(x, y, z);
    bs.ResetReadPointer();
    float ox, oy, oz;
    EXPECT_TRUE(bs.ReadNormVector(ox, oy, oz));
    EXPECT_NEAR(1.0f, ox, 1.0f / 32767.0f);
    EXPECT_NEAR(0.0f, oy, 1.0f / 32767.0f);
    EXPECT_NEAR(0.0f, oz, 1.0f / 32767.0f);
}

// Verify NormVector round-trip with a diagonal vector (1,1,1) which
// normalizes to ~(0.577, 0.577, 0.577). Tests that all three components
// survive encoding with the expected precision.
TEST(MockBitStream, NormVectorRoundTrip_Diagonal)
{
    MockBitStream bs;
    float         s = 1.0f / std::sqrt(3.0f);
    bs.WriteNormVector(s, s, s);
    bs.ResetReadPointer();
    float ox, oy, oz;
    EXPECT_TRUE(bs.ReadNormVector(ox, oy, oz));
    EXPECT_NEAR(s, ox, 1.0f / 32767.0f + 1e-5f);
    EXPECT_NEAR(s, oy, 1.0f / 32767.0f + 1e-5f);
    EXPECT_NEAR(s, oz, 1.0f / 32767.0f + 1e-5f);
}

// Verify that reading from an empty stream returns false instead of
// producing undefined behavior.
TEST(MockBitStream, ReadBeyondEnd_ReturnsFalse)
{
    MockBitStream bs;
    unsigned char val = 0;
    EXPECT_FALSE(bs.Read(val));
}

// Verify that Reset() clears both the write position and read offset,
// effectively emptying the stream.
TEST(MockBitStream, Reset_ClearsEverything)
{
    MockBitStream bs;
    bs.Write(static_cast<unsigned int>(0xDEADBEEF));
    EXPECT_EQ(32, bs.GetNumberOfBitsUsed());
    bs.Reset();
    EXPECT_EQ(0, bs.GetNumberOfBitsUsed());
    EXPECT_EQ(0, bs.GetReadOffsetAsBits());
}

// Verify that Write(ISyncStructure*) and Read(ISyncStructure*) correctly
// delegate to the sync structure's own Write/Read methods. Uses
// SPlayerHealthSync as a concrete ISyncStructure.
TEST(MockBitStream, ISyncStructureDelegation)
{
    MockBitStream     bs;
    SPlayerHealthSync writeSync;
    writeSync.data.fValue = 100.0f;
    bs.Write(&writeSync);
    bs.ResetReadPointer();
    SPlayerHealthSync readSync;
    EXPECT_TRUE(bs.Read(&readSync));
    EXPECT_NEAR(100.0f, readSync.data.fValue, 1.5f);
}
