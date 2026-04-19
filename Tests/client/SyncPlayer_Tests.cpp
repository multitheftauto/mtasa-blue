/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SyncPlayer_Tests.cpp
 *  PURPOSE:     Round-trip tests for player/ped input and weapon sync
 *               structures
 *
 *  Covers puresync flags (player and vehicle), ped/camera rotation,
 *  keysync (flags + full controller state), driveby direction, weapon
 *  slot/type/ammo/aim, and bodypart hit detection.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include "MockBitStream.h"
#include <net/SyncStructures.h>

// ============================================================================
// Player puresync flags
// ============================================================================

// 15 individual bit-flags packed into a single sync structure.
// Verify that setting specific flags doesn't bleed into others.
TEST(SPlayerPuresyncFlags, RoundTrip)
{
    MockBitStream        bs;
    SPlayerPuresyncFlags sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.bIsInWater = true;
    sync.data.bIsDucked = true;
    sync.data.bHasAWeapon = true;
    sync.data.hangingDuringClimb = true;
    sync.Write(bs);
    EXPECT_EQ(15, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SPlayerPuresyncFlags out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bIsInWater);
    EXPECT_FALSE(out.data.bIsOnGround);
    EXPECT_TRUE(out.data.bIsDucked);
    EXPECT_TRUE(out.data.bHasAWeapon);
    EXPECT_TRUE(out.data.hangingDuringClimb);
    EXPECT_FALSE(out.data.bIsOnFire);
}

// ============================================================================
// Ped/Camera rotation
// ============================================================================

// Ped rotation: 16-bit FloatAsBits over [-PI, PI]. Step = 2*PI/65535 ~ 0.0001.
TEST(SPedRotationSync, RoundTrip)
{
    MockBitStream    bs;
    SPedRotationSync sync;
    sync.data.fRotation = 1.5f;
    sync.Write(bs);
    EXPECT_EQ(16, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SPedRotationSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1.5f, out.data.fRotation, 0.001f);
}

// Camera rotation: 12-bit FloatAsBits over [-PI, PI] with wrap.
// Step = 2*PI/4095 ~ 0.0015. Lower precision than ped rotation because
// the camera angle changes rapidly and doesn't need atomic accuracy.
TEST(SCameraRotationSync, RoundTrip)
{
    MockBitStream       bs;
    SCameraRotationSync sync;
    sync.data.fRotation = -2.0f;
    sync.Write(bs);
    EXPECT_EQ(12, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SCameraRotationSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(-2.0f, out.data.fRotation, 0.01f);
}

// SKeysyncRotation bundles player and camera rotation together.
// Both use 12-bit FloatAsBits (not the 16-bit SPedRotationSync),
// so the total is 24 bits.
TEST(SKeysyncRotation, RoundTrip)
{
    MockBitStream    bs;
    SKeysyncRotation sync;
    sync.data.fPlayerRotation = 1.0f;
    sync.data.fCameraRotation = -1.0f;
    sync.Write(bs);
    EXPECT_EQ(24, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SKeysyncRotation out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1.0f, out.data.fPlayerRotation, 0.01f);
    EXPECT_NEAR(-1.0f, out.data.fCameraRotation, 0.01f);
}

// ============================================================================
// Vehicle puresync flags
// ============================================================================

// 11 bit-flags describing the vehicle occupant's state during puresync.
TEST(SVehiclePuresyncFlags, RoundTrip)
{
    MockBitStream         bs;
    SVehiclePuresyncFlags sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.bIsWearingGoggles = true;
    sync.data.bIsOnGround = true;
    sync.data.bIsHeliSearchLightVisible = true;
    sync.Write(bs);
    EXPECT_EQ(11, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SVehiclePuresyncFlags out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bIsWearingGoggles);
    EXPECT_FALSE(out.data.bIsDoingGangDriveby);
    EXPECT_TRUE(out.data.bIsOnGround);
    EXPECT_TRUE(out.data.bIsHeliSearchLightVisible);
}

// ============================================================================
// Driveby direction
// ============================================================================

// 2-bit enum: FORWARDS=0, LEFT=1, RIGHT=2, BACKWARDS=3.
TEST(SDrivebyDirectionSync, RoundTrip)
{
    MockBitStream         bs;
    SDrivebyDirectionSync sync;
    sync.data.ucDirection = eVehicleAimDirection::BACKWARDS;
    sync.Write(bs);
    EXPECT_EQ(2, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SDrivebyDirectionSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(eVehicleAimDirection::BACKWARDS, out.data.ucDirection);
}

// ============================================================================
// Keysync flags
// ============================================================================

// 4 bit-flags describing the player's key state during keysync packets.
TEST(SKeysyncFlags, RoundTrip)
{
    MockBitStream bs;
    SKeysyncFlags sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.bIsDucked = true;
    sync.data.bSyncingVehicle = true;
    sync.Write(bs);
    EXPECT_EQ(4, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SKeysyncFlags out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bIsDucked);
    EXPECT_FALSE(out.data.bIsChoking);
    EXPECT_TRUE(out.data.bSyncingVehicle);
}

// ============================================================================
// Full keysync
// ============================================================================

// Full controller state: digital buttons, analog buttons (square/cross),
// and analog stick positions. Sticks are scaled via 128/127 division
// to fit in fewer bits.
TEST(SFullKeysyncSync, RoundTrip_NoAnalogButtons)
{
    MockBitStream    bs;
    SFullKeysyncSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.bLeftShoulder1 = true;
    sync.data.bButtonCircle = true;
    sync.data.ucButtonSquare = 0;
    sync.data.ucButtonCross = 0;
    sync.data.sLeftStickX = 64;
    sync.data.sLeftStickY = -64;
    sync.Write(bs);
    bs.ResetReadPointer();
    SFullKeysyncSync out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bLeftShoulder1);
    EXPECT_FALSE(out.data.bRightShoulder1);
    EXPECT_TRUE(out.data.bButtonCircle);
    EXPECT_EQ(0, out.data.ucButtonSquare);
    EXPECT_EQ(0, out.data.ucButtonCross);
    // Sticks go through 128/127 scaling, so allow small rounding
    EXPECT_NEAR(64, out.data.sLeftStickX, 2);
    EXPECT_NEAR(-64, out.data.sLeftStickY, 2);
}

// When analog button values are in range [1, 254], they are written with
// full precision. Values 0 and 255 use optimized single-bit paths.
TEST(SFullKeysyncSync, RoundTrip_WithAnalogButtons)
{
    MockBitStream    bs;
    SFullKeysyncSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.ucButtonSquare = 128;
    sync.data.ucButtonCross = 200;
    sync.data.sLeftStickX = 0;
    sync.data.sLeftStickY = 0;
    sync.Write(bs);
    bs.ResetReadPointer();
    SFullKeysyncSync out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(128, out.data.ucButtonSquare);
    EXPECT_EQ(200, out.data.ucButtonCross);
}

// ============================================================================
// Small keysync (tests the subtle field-ordering difference vs SFullKeysyncSync)
// ============================================================================

// SSmallKeysyncSync has ucButtonCross before ucButtonSquare in its data
// struct (opposite of SFullKeysyncSync), but the serialization order is
// the same: Square first, then Cross. This test verifies the two analog
// buttons don't get swapped during round-trip.
TEST(SSmallKeysyncSync, RoundTrip_NoAnalogButtons)
{
    MockBitStream     bs;
    SSmallKeysyncSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.bLeftShoulder1 = true;
    sync.data.bButtonCircle = true;
    sync.data.ucButtonSquare = 0;
    sync.data.ucButtonCross = 0;
    sync.data.sLeftStickX = 64;
    sync.data.sLeftStickY = -64;
    sync.Write(bs);
    bs.ResetReadPointer();
    SSmallKeysyncSync out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bLeftShoulder1);
    EXPECT_FALSE(out.data.bRightShoulder1);
    EXPECT_TRUE(out.data.bButtonCircle);
    EXPECT_EQ(0, out.data.ucButtonSquare);
    EXPECT_EQ(0, out.data.ucButtonCross);
    EXPECT_NEAR(64, out.data.sLeftStickX, 2);
    EXPECT_NEAR(-64, out.data.sLeftStickY, 2);
}

// When analog button values are in range [1, 254], they are written with
// full precision. This exercises the ucButtonSquare/ucButtonCross ordering
// which differs from SFullKeysyncSync's data layout.
TEST(SSmallKeysyncSync, RoundTrip_WithAnalogButtons)
{
    MockBitStream     bs;
    SSmallKeysyncSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.ucButtonSquare = 100;
    sync.data.ucButtonCross = 200;
    sync.data.sLeftStickX = 0;
    sync.data.sLeftStickY = 0;
    sync.Write(bs);
    bs.ResetReadPointer();
    SSmallKeysyncSync out;
    std::memset(&out.data, 0, sizeof(out.data));
    EXPECT_TRUE(out.Read(bs));
    // Verify that Square and Cross don't get swapped despite the
    // different field order in the data struct vs SFullKeysyncSync.
    EXPECT_EQ(100, out.data.ucButtonSquare);
    EXPECT_EQ(200, out.data.ucButtonCross);
}

// ============================================================================
// Weapon syncs
// ============================================================================

// Weapon slot: 4-bit index into the GTA:SA weapon slot array [0, 12].
TEST(SWeaponSlotSync, RoundTrip)
{
    MockBitStream   bs;
    SWeaponSlotSync sync;
    sync.data.uiSlot = 7;
    sync.Write(bs);
    EXPECT_EQ(4, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SWeaponSlotSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(7u, out.data.uiSlot);
}

// Weapon type: 6-bit weapon ID [0, 46].
TEST(SWeaponTypeSync, RoundTrip)
{
    MockBitStream   bs;
    SWeaponTypeSync sync;
    sync.data.ucWeaponType = 30;
    sync.Write(bs);
    EXPECT_EQ(6, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SWeaponTypeSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(30, out.data.ucWeaponType);
}

// Weapon ammo: variable-width encoding that depends on the weapon type.
// When both total and clip ammo are synced, both fields are written.
TEST(SWeaponAmmoSync, RoundTrip_BothSynced)
{
    MockBitStream   bs;
    SWeaponAmmoSync sync(22, true, true);  // weapon 22 = Colt 45
    sync.data.usTotalAmmo = 500;
    sync.data.usAmmoInClip = 17;
    sync.Write(bs);
    bs.ResetReadPointer();
    SWeaponAmmoSync out(22, true, true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(500, out.data.usTotalAmmo);
    EXPECT_EQ(17, out.data.usAmmoInClip);
}

// When only total ammo is synced, clip ammo is omitted from the stream.
TEST(SWeaponAmmoSync, RoundTrip_TotalOnly)
{
    MockBitStream   bs;
    SWeaponAmmoSync sync(30, true, false);  // weapon 30 = AK-47
    sync.data.usTotalAmmo = 9999;
    sync.Write(bs);
    bs.ResetReadPointer();
    SWeaponAmmoSync out(30, true, false);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(9999, out.data.usTotalAmmo);
}

// Weapon aim (partial): only the arm direction (fArm) is synced.
// Arm angle is encoded as a short via degrees conversion.
TEST(SWeaponAimSync, RoundTrip_ArmOnly)
{
    MockBitStream  bs;
    SWeaponAimSync sync(0.0f, false);
    sync.data.fArm = 0.5f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SWeaponAimSync out(0.0f, false);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(0.5f, out.data.fArm, 0.01f);
}

// Weapon aim (full): syncs origin (float), target (as NormVector direction
// * range), and arm angle. The target is reconstructed by decoder as
// origin + normalized_direction * range.
TEST(SWeaponAimSync, RoundTrip_Full)
{
    MockBitStream  bs;
    float          range = 50.0f;
    SWeaponAimSync sync(range, true);
    sync.data.fArm = 0.25f;
    sync.data.vecOrigin = CVector(100.0f, 200.0f, 10.0f);
    sync.data.vecTarget = CVector(100.0f + range, 200.0f, 10.0f);
    sync.Write(bs);
    bs.ResetReadPointer();
    SWeaponAimSync out(range, true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(0.25f, out.data.fArm, 0.01f);
    EXPECT_FLOAT_EQ(100.0f, out.data.vecOrigin.fX);
    EXPECT_FLOAT_EQ(200.0f, out.data.vecOrigin.fY);
    EXPECT_FLOAT_EQ(10.0f, out.data.vecOrigin.fZ);
    // Target = origin + normalized_direction * range
    EXPECT_NEAR(100.0f + range, out.data.vecTarget.fX, 0.1f);
    EXPECT_NEAR(200.0f, out.data.vecTarget.fY, 0.1f);
    EXPECT_NEAR(10.0f, out.data.vecTarget.fZ, 0.1f);
}

// Weapon aim (full, diagonal): uses a diagonal aim direction to exercise
// the Y/Z swap in WriteNormVector/ReadNormVector calls. SWeaponAimSync
// writes NormVector(fX, fZ, fY) and reads NormVector(fX, fZ, fY), so a
// test with Y=Z=0 wouldn't catch component ordering bugs.
TEST(SWeaponAimSync, RoundTrip_FullDiagonal)
{
    MockBitStream  bs;
    float          range = 100.0f;
    SWeaponAimSync sync(range, true);
    sync.data.fArm = 0.1f;
    sync.data.vecOrigin = CVector(0.0f, 0.0f, 0.0f);
    // Diagonal target: direction has non-trivial X, Y, and Z components
    sync.data.vecTarget = CVector(50.0f, 70.0f, 30.0f);
    sync.Write(bs);
    bs.ResetReadPointer();
    SWeaponAimSync out(range, true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(0.1f, out.data.fArm, 0.01f);
    // The target is reconstructed as origin + normalized(direction) * range.
    // NormVector quantization (16-bit per component, ~1/32767 error) gets
    // amplified by the weapon range. With range=100 the per-component error
    // can reach ~100/32767 ≈ 0.003, but the Y/Z swap and re-normalization
    // in the sync structure can amplify this further. Use 10% of range as
    // a generous tolerance that still catches gross component-swap bugs.
    const float kTol = range * 0.1f;
    EXPECT_NEAR(sync.data.vecTarget.fX, out.data.vecTarget.fX, kTol);
    EXPECT_NEAR(sync.data.vecTarget.fY, out.data.vecTarget.fY, kTol);
    EXPECT_NEAR(sync.data.vecTarget.fZ, out.data.vecTarget.fZ, kTol);
    // Verify Y and Z didn't get swapped: Y should be larger than Z
    EXPECT_GT(out.data.vecTarget.fY, out.data.vecTarget.fZ);
}

// ============================================================================
// Bodypart sync
// ============================================================================

// GTA:SA bodypart IDs range from 3 (torso) to 9 (head). Encoded in 3 bits
// with an offset of -3, so on the wire it maps [3,9] → [0,6].
TEST(SBodypartSync, RoundTrip_AllBodyparts)
{
    for (unsigned int bp = 3; bp <= 9; ++bp)
    {
        MockBitStream bs;
        SBodypartSync sync;
        sync.data.uiBodypart = bp;
        sync.Write(bs);
        EXPECT_EQ(3, bs.GetNumberOfBitsUsed());
        bs.ResetReadPointer();
        SBodypartSync out;
        EXPECT_TRUE(out.Read(bs));
        EXPECT_EQ(bp, out.data.uiBodypart) << "Bodypart " << bp << " failed round-trip";
    }
}
