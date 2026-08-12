/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SyncMisc_Tests.cpp
 *  PURPOSE:     Round-trip tests for miscellaneous sync structures
 *
 *  Covers explosion type, map info flags, fun bugs state, world special
 *  properties, quit reason, entity alpha, marker/pickup/colshape types,
 *  RGBA color, occupied seat, paintjob, override lights, lua type,
 *  mouse button, heat haze, and weapon property sync.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include "MockBitStream.h"
#include <net/SyncStructures.h>

// ============================================================================
// Explosion type
// ============================================================================

// 4-bit explosion type ID [0, 15].
TEST(SExplosionTypeSync, RoundTrip)
{
    MockBitStream      bs;
    SExplosionTypeSync sync;
    sync.data.uiType = 12;
    sync.Write(bs);
    EXPECT_EQ(4, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SExplosionTypeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(12u, out.data.uiType);
}

// ============================================================================
// Map info flags
// ============================================================================

// 3 independent boolean flags sent during map start. Each is 1 bit.
TEST(SMapInfoFlagsSync, RoundTrip)
{
    MockBitStream     bs;
    SMapInfoFlagsSync sync;
    sync.data.bShowNametags = true;
    sync.data.bShowRadar = false;
    sync.data.bCloudsEnabled = true;
    sync.Write(bs);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SMapInfoFlagsSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bShowNametags);
    EXPECT_FALSE(out.data.bShowRadar);
    EXPECT_TRUE(out.data.bCloudsEnabled);
}

// ============================================================================
// Fun bugs state
// ============================================================================

// 10 boolean flags controlling GTA:SA gameplay bugs that servers can
// individually enable/disable. Verify that setting specific flags doesn't
// affect unrelated flags.
TEST(SFunBugsStateSync, RoundTrip)
{
    MockBitStream     bs;
    SFunBugsStateSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.bCloseRangeDamage = true;
    sync.data.bFastFire = true;
    sync.data.vehicleRapidStop = true;
    sync.Write(bs);
    EXPECT_EQ(10, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SFunBugsStateSync out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bCloseRangeDamage);
    EXPECT_FALSE(out.data.bQuickReload);
    EXPECT_TRUE(out.data.bFastFire);
    EXPECT_TRUE(out.data.vehicleRapidStop);
}

// ============================================================================
// World special properties
// ============================================================================

// 20 boolean flags controlling world behaviour (hovercars, foliage, etc.).
// The constructor sets default values, so we flip a few and verify both
// changed and unchanged flags.
TEST(SWorldSpecialPropertiesStateSync, RoundTrip)
{
    MockBitStream                    bs;
    SWorldSpecialPropertiesStateSync sync;
    sync.data.hovercars = true;
    sync.data.randomfoliage = false;
    sync.data.vehicleEngineAutoStart = false;
    sync.Write(bs);
    EXPECT_EQ(20, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SWorldSpecialPropertiesStateSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.hovercars);
    EXPECT_FALSE(out.data.randomfoliage);
    EXPECT_FALSE(out.data.vehicleEngineAutoStart);
    // Verify a default-true flag that wasn't changed
    EXPECT_TRUE(out.data.underworldwarp);
}

// ============================================================================
// Quit reason
// ============================================================================

// 3-bit quit reason code [0, 7].
TEST(SQuitReasonSync, RoundTrip)
{
    MockBitStream   bs;
    SQuitReasonSync sync;
    sync.data.uiQuitReason = 5;
    sync.Write(bs);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SQuitReasonSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(5u, out.data.uiQuitReason);
}

// ============================================================================
// Entity alpha (compressed)
// ============================================================================

// Entity alpha uses WriteCompressed/ReadCompressed on an unsigned char.
// Since it's a single byte, all 8 bits are written without a compression flag.

// Fully opaque (255) - the most common case.
TEST(SEntityAlphaSync, RoundTrip_Opaque)
{
    MockBitStream    bs;
    SEntityAlphaSync sync;
    sync.data.ucAlpha = 255;
    sync.Write(bs);
    bs.ResetReadPointer();
    SEntityAlphaSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(255, out.data.ucAlpha);
}

// Half-transparent (128).
TEST(SEntityAlphaSync, RoundTrip_HalfTransparent)
{
    MockBitStream    bs;
    SEntityAlphaSync sync;
    sync.data.ucAlpha = 128;
    sync.Write(bs);
    bs.ResetReadPointer();
    SEntityAlphaSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(128, out.data.ucAlpha);
}

// Fully transparent (0) - edge case, entity is invisible.
TEST(SEntityAlphaSync, RoundTrip_FullyTransparent)
{
    MockBitStream    bs;
    SEntityAlphaSync sync;
    sync.data.ucAlpha = 0;
    sync.Write(bs);
    bs.ResetReadPointer();
    SEntityAlphaSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(0, out.data.ucAlpha);
}

// ============================================================================
// Marker / Pickup / Colshape type syncs
// ============================================================================

// All three use SIntegerSync with 3 bits (range [0, 7]).

// Marker types: checkpoint, ring, cylinder, arrow, corona (0-4).
TEST(SMarkerTypeSync, RoundTrip)
{
    MockBitStream   bs;
    SMarkerTypeSync sync;
    sync.data.ucType = 5;
    sync.Write(bs);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SMarkerTypeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(5, out.data.ucType);
}

// Pickup types: health, armor, weapon, custom (0-3).
TEST(SPickupTypeSync, RoundTrip)
{
    MockBitStream   bs;
    SPickupTypeSync sync;
    sync.data.ucType = 3;
    sync.Write(bs);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SPickupTypeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(3, out.data.ucType);
}

// Colshape types: cuboid, sphere, circle, polygon, tube (0-4).
TEST(SColshapeTypeSync, RoundTrip)
{
    MockBitStream     bs;
    SColshapeTypeSync sync;
    sync.data.ucType = 4;
    sync.Write(bs);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SColshapeTypeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(4, out.data.ucType);
}

// ============================================================================
// Color sync
// ============================================================================

// Full RGBA color: 4 × 8 bits = 32 bits. No quantization.
TEST(SColorSync, RoundTrip)
{
    MockBitStream bs;
    SColorSync    sync;
    sync.data.ucR = 255;
    sync.data.ucG = 128;
    sync.data.ucB = 0;
    sync.data.ucA = 200;
    sync.Write(bs);
    EXPECT_EQ(32, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SColorSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(255, out.data.ucR);
    EXPECT_EQ(128, out.data.ucG);
    EXPECT_EQ(0, out.data.ucB);
    EXPECT_EQ(200, out.data.ucA);
}

// ============================================================================
// Occupied seat / Paintjob / Override lights
// ============================================================================

// Occupied seat: 4-bit value [0, 8] for the seat index.
TEST(SOccupiedSeatSync, RoundTrip)
{
    MockBitStream     bs;
    SOccupiedSeatSync sync;
    sync.data.ucSeat = 3;
    sync.Write(bs);
    EXPECT_EQ(4, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SOccupiedSeatSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(3, out.data.ucSeat);
}

// Paintjob: 2-bit value [0, 3].
TEST(SPaintjobSync, RoundTrip)
{
    MockBitStream bs;
    SPaintjobSync sync;
    sync.data.ucPaintjob = 2;
    sync.Write(bs);
    EXPECT_EQ(2, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SPaintjobSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(2, out.data.ucPaintjob);
}

// Override lights: 2-bit value (0=default, 1=force off, 2=force on).
TEST(SOverrideLightsSync, RoundTrip)
{
    MockBitStream       bs;
    SOverrideLightsSync sync;
    sync.data.ucOverride = 1;
    sync.Write(bs);
    EXPECT_EQ(2, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SOverrideLightsSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(1, out.data.ucOverride);
}

// ============================================================================
// Lua type / Mouse button
// ============================================================================

// Lua type: 4-bit value corresponding to Lua type constants.
TEST(SLuaTypeSync, RoundTrip)
{
    MockBitStream bs;
    SLuaTypeSync  sync;
    sync.data.ucType = 9;
    sync.Write(bs);
    EXPECT_EQ(4, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SLuaTypeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(9, out.data.ucType);
}

// Mouse button: 3-bit value (left=1, middle=2, right=3, etc.).
TEST(SMouseButtonSync, RoundTrip)
{
    MockBitStream    bs;
    SMouseButtonSync sync;
    sync.data.ucButton = 5;
    sync.Write(bs);
    EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SMouseButtonSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(5, out.data.ucButton);
}

// ============================================================================
// Heat haze
// ============================================================================

// Heat haze settings: ucIntensity and ucRandomShift are raw 8-bit values,
// but the speed, scan size, and render size fields use bit-truncated
// ReadRange/WriteRange (10-bit or 11-bit). bInsideBuilding is a single bit.
// Test values are chosen to fit within the truncated ranges so they
// round-trip exactly.
// Total: 2×8 + 4×10 + 2×11 + 1 = 79 bits.
TEST(SHeatHazeSync, RoundTrip)
{
    MockBitStream bs;
    SHeatHazeSync sync;
    sync.data.settings.ucIntensity = 100;
    sync.data.settings.ucRandomShift = 50;
    sync.data.settings.usSpeedMin = 200;
    sync.data.settings.usSpeedMax = 800;
    sync.data.settings.sScanSizeX = -500;
    sync.data.settings.sScanSizeY = 300;
    sync.data.settings.usRenderSizeX = 400;
    sync.data.settings.usRenderSizeY = 600;
    sync.data.settings.bInsideBuilding = true;
    sync.Write(bs);
    EXPECT_EQ(79, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SHeatHazeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(100, out.data.settings.ucIntensity);
    EXPECT_EQ(50, out.data.settings.ucRandomShift);
    EXPECT_EQ(200, out.data.settings.usSpeedMin);
    EXPECT_EQ(800, out.data.settings.usSpeedMax);
    EXPECT_EQ(-500, out.data.settings.sScanSizeX);
    EXPECT_EQ(300, out.data.settings.sScanSizeY);
    EXPECT_EQ(400, out.data.settings.usRenderSizeX);
    EXPECT_EQ(600, out.data.settings.usRenderSizeY);
    EXPECT_TRUE(out.data.settings.bInsideBuilding);
}

// ============================================================================
// Weapon property sync
// ============================================================================

// sWeaponPropertySync: the full weapon stats configuration. All fields are
// raw floats/ints, so they should survive the round-trip exactly.
// Total: 2×int(32) + 2×short(16) + 9×float(32) = 384 bits.
TEST(sWeaponPropertySync, RoundTrip)
{
    MockBitStream       bs;
    sWeaponPropertySync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.weaponType = 22;
    sync.data.fTargetRange = 35.0f;
    sync.data.fWeaponRange = 35.0f;
    sync.data.nFlags = 0x1234;
    sync.data.nAmmo = 17;
    sync.data.nDamage = 25;
    sync.data.fAccuracy = 1.5f;
    sync.data.fMoveSpeed = 1.0f;
    sync.data.anim_loop_start = 0.1f;
    sync.data.anim_loop_stop = 0.5f;
    sync.data.anim_loop_bullet_fire = 0.2f;
    sync.data.anim2_loop_start = 0.0f;
    sync.data.anim2_loop_stop = 0.0f;
    sync.data.anim2_loop_bullet_fire = 0.0f;
    sync.data.anim_breakout_time = 0.3f;
    sync.Write(bs);
    // int + float + float + int + short + short + float + float + 7×float
    // = 32 + 32 + 32 + 32 + 16 + 16 + 32 + 32 + 7×32 = 448 bits
    EXPECT_EQ(448, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    sWeaponPropertySync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(22, out.data.weaponType);
    EXPECT_FLOAT_EQ(35.0f, out.data.fTargetRange);
    EXPECT_FLOAT_EQ(35.0f, out.data.fWeaponRange);
    EXPECT_EQ(0x1234, out.data.nFlags);
    EXPECT_EQ(17, out.data.nAmmo);
    EXPECT_EQ(25, out.data.nDamage);
    EXPECT_FLOAT_EQ(1.5f, out.data.fAccuracy);
    EXPECT_FLOAT_EQ(0.3f, out.data.anim_breakout_time);
}
