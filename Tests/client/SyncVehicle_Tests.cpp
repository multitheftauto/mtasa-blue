/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/SyncVehicle_Tests.cpp
 *  PURPOSE:     Round-trip tests for vehicle-specific sync structures
 *
 *  Covers vehicle turret, door open ratio, vehicle part damage (doors,
 *  wheels, panels, lights), sirens, handling, unoccupied vehicle sync,
 *  and the composite SVehicleDamageSyncMethodeB.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include "MockBitStream.h"
#include <net/SyncStructures.h>

// ============================================================================
// Vehicle turret
// ============================================================================

// Turret angles are converted to shorts scaled by 32767/PI, giving
// a precision of ~0.0001 radians per step.
TEST(SVehicleTurretSync, RoundTrip)
{
    MockBitStream      bs;
    SVehicleTurretSync sync;
    sync.data.fTurretX = 1.5f;
    sync.data.fTurretY = -0.5f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehicleTurretSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(1.5f, out.data.fTurretX, 0.001f);
    EXPECT_NEAR(-0.5f, out.data.fTurretY, 0.001f);
}

// ============================================================================
// Door open ratio
// ============================================================================

// Uses a compressed path for common values 0.0 and 1.0 (2 bits each),
// and a 10-bit FloatAsBits for intermediate values.

// Fully closed: compressed path = 1 bit flag + 1 bit value = 2 bits.
TEST(SDoorOpenRatioSync, RoundTrip_Zero)
{
    MockBitStream      bs;
    SDoorOpenRatioSync sync;
    sync.data.fRatio = 0.0f;
    sync.Write(bs);
    EXPECT_EQ(2, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SDoorOpenRatioSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(0.0f, out.data.fRatio);
}

// Fully open: also uses the compressed path.
TEST(SDoorOpenRatioSync, RoundTrip_One)
{
    MockBitStream      bs;
    SDoorOpenRatioSync sync;
    sync.data.fRatio = 1.0f;
    sync.Write(bs);
    EXPECT_EQ(2, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SDoorOpenRatioSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(1.0f, out.data.fRatio);
}

// Partially open: uncompressed path = 1 bit flag + 10 bits FloatAsBits = 11 bits.
// 10 bits over [0,1] gives step ~0.001.
TEST(SDoorOpenRatioSync, RoundTrip_Partial)
{
    MockBitStream      bs;
    SDoorOpenRatioSync sync;
    sync.data.fRatio = 0.5f;
    sync.Write(bs);
    EXPECT_EQ(11, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SDoorOpenRatioSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_NEAR(0.5f, out.data.fRatio, 0.01f);
}

// ============================================================================
// Vehicle damage syncs
// ============================================================================

// SVehiclePartStateSync<COUNT, BITS>: N parts × BITS bits per part.
// Has two modes: delta (only changed parts) and full (all parts).

// Full sync (no delta): all 6 door states are written unconditionally.
// Each door state uses 3 bits (range [0,7]).
TEST(SVehiclePartStateSync, RoundTrip_NoDelta)
{
    MockBitStream                       bs;
    SVehiclePartStateSync<MAX_DOORS, 3> sync(false);
    for (unsigned int i = 0; i < MAX_DOORS; ++i)
    {
        sync.data.bChanged[i] = true;
        sync.data.ucStates[i] = i % 8;
    }
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehiclePartStateSync<MAX_DOORS, 3> out(false);
    EXPECT_TRUE(out.Read(bs));
    for (unsigned int i = 0; i < MAX_DOORS; ++i)
    {
        EXPECT_EQ(i % 8, out.data.ucStates[i]) << "Door " << i;
    }
}

// Delta sync: only parts with bChanged=true are written. Saves bandwidth
// when only one or two parts changed since last sync.
TEST(SVehiclePartStateSync, RoundTrip_WithDelta)
{
    MockBitStream                        bs;
    SVehiclePartStateSync<MAX_WHEELS, 2> sync(true);
    sync.data.bChanged[0] = true;
    sync.data.ucStates[0] = 2;
    sync.data.bChanged[1] = false;
    sync.data.ucStates[1] = 0;
    sync.data.bChanged[2] = true;
    sync.data.ucStates[2] = 3;
    sync.data.bChanged[3] = false;
    sync.data.ucStates[3] = 0;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehiclePartStateSync<MAX_WHEELS, 2> out(true);
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bChanged[0]);
    EXPECT_EQ(2, out.data.ucStates[0]);
    EXPECT_FALSE(out.data.bChanged[1]);
    EXPECT_TRUE(out.data.bChanged[2]);
    EXPECT_EQ(3, out.data.ucStates[2]);
}

// SVehiclePartStateSyncMethodeB: alternative encoding that writes a
// "non-zero" flag bit. If all states are zero, only 1 bit is used.
// Otherwise all states are written.
TEST(SVehiclePartStateSyncMethodeB, RoundTrip_NonZero)
{
    MockBitStream                               bs;
    SVehiclePartStateSyncMethodeB<MAX_DOORS, 3> sync;
    sync.data.ucStates[0] = 1;
    sync.data.ucStates[1] = 3;
    sync.data.ucStates[2] = 7;
    sync.data.ucStates[3] = 0;
    sync.data.ucStates[4] = 5;
    sync.data.ucStates[5] = 2;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehiclePartStateSyncMethodeB<MAX_DOORS, 3> out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(1, out.data.ucStates[0]);
    EXPECT_EQ(3, out.data.ucStates[1]);
    EXPECT_EQ(7, out.data.ucStates[2]);
    EXPECT_EQ(0, out.data.ucStates[3]);
    EXPECT_EQ(5, out.data.ucStates[4]);
    EXPECT_EQ(2, out.data.ucStates[5]);
}

// All-zero optimization: when every state is 0, only 1 bit is written.
TEST(SVehiclePartStateSyncMethodeB, RoundTrip_AllZero)
{
    MockBitStream                                bs;
    SVehiclePartStateSyncMethodeB<MAX_WHEELS, 2> sync;
    std::memset(&sync.data.ucStates[0], 0, MAX_WHEELS);
    sync.Write(bs);
    EXPECT_EQ(1, bs.GetNumberOfBitsUsed());
    bs.ResetReadPointer();
    SVehiclePartStateSyncMethodeB<MAX_WHEELS, 2> out;
    EXPECT_TRUE(out.Read(bs));
    for (unsigned int i = 0; i < MAX_WHEELS; ++i)
        EXPECT_EQ(0, out.data.ucStates[i]);
}

// ============================================================================
// Vehicle siren syncs
// ============================================================================

// SVehicleSirenAddSync: metadata about the siren configuration (type,
// count, behavioral flags). Sent when adding sirens to a vehicle.
TEST(SVehicleSirenAddSync, RoundTrip)
{
    MockBitStream        bs;
    SVehicleSirenAddSync sync;
    sync.data.m_bOverrideSirens = true;
    sync.data.m_ucSirenType = 3;
    sync.data.m_ucSirenCount = 6;
    sync.data.m_b360Flag = true;
    sync.data.m_bDoLOSCheck = false;
    sync.data.m_bUseRandomiser = true;
    sync.data.m_bEnableSilent = false;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehicleSirenAddSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.m_bOverrideSirens);
    EXPECT_EQ(3, out.data.m_ucSirenType);
    EXPECT_EQ(6, out.data.m_ucSirenCount);
    EXPECT_TRUE(out.data.m_b360Flag);
    EXPECT_FALSE(out.data.m_bDoLOSCheck);
    EXPECT_TRUE(out.data.m_bUseRandomiser);
    EXPECT_FALSE(out.data.m_bEnableSilent);
}

// SVehicleSirenSync: per-siren point data including 3D position, color,
// min alpha, and behavioral flags.
TEST(SVehicleSirenSync, RoundTrip)
{
    MockBitStream     bs;
    SVehicleSirenSync sync;
    sync.data.m_bOverrideSirens = true;
    sync.data.m_ucSirenID = 2;
    sync.data.m_vecSirenPositions = CVector(1.0f, 2.0f, 3.0f);
    sync.data.m_colSirenColour = SColorRGBA(255, 0, 0, 200);
    sync.data.m_dwSirenMinAlpha = 50;
    sync.data.m_b360Flag = false;
    sync.data.m_bDoLOSCheck = true;
    sync.data.m_bUseRandomiser = false;
    sync.data.m_bEnableSilent = true;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehicleSirenSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.m_bOverrideSirens);
    EXPECT_EQ(2, out.data.m_ucSirenID);
    EXPECT_FLOAT_EQ(1.0f, out.data.m_vecSirenPositions.fX);
    EXPECT_FLOAT_EQ(2.0f, out.data.m_vecSirenPositions.fY);
    EXPECT_FLOAT_EQ(3.0f, out.data.m_vecSirenPositions.fZ);
    EXPECT_EQ(200, out.data.m_colSirenColour.A);
    EXPECT_EQ(255, out.data.m_colSirenColour.R);
    EXPECT_EQ(0, out.data.m_colSirenColour.G);
    EXPECT_EQ(0, out.data.m_colSirenColour.B);
    EXPECT_EQ(50u, out.data.m_dwSirenMinAlpha);
    EXPECT_FALSE(out.data.m_b360Flag);
    EXPECT_TRUE(out.data.m_bDoLOSCheck);
}

// ============================================================================
// Vehicle handling sync (large struct, all scalar reads)
// ============================================================================

// SVehicleHandlingSync: the full vehicle handling configuration. All fields
// are raw scalars (no quantization), so they should round-trip exactly.
// This is one of the largest sync structures in the protocol.
TEST(SVehicleHandlingSync, RoundTrip)
{
    MockBitStream        bs;
    SVehicleHandlingSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.fMass = 1500.0f;
    sync.data.fTurnMass = 4000.0f;
    sync.data.fDragCoeff = 2.5f;
    sync.data.vecCenterOfMass = CVector(0.0f, 0.1f, -0.2f);
    sync.data.ucPercentSubmerged = 75;
    sync.data.fTractionMultiplier = 0.7f;
    sync.data.ucDriveType = 'R';
    sync.data.ucEngineType = 'P';
    sync.data.ucNumberOfGears = 5;
    sync.data.fEngineAcceleration = 10.0f;
    sync.data.fEngineInertia = 5.0f;
    sync.data.fMaxVelocity = 200.0f;
    sync.data.fBrakeDeceleration = 8.0f;
    sync.data.fBrakeBias = 0.52f;
    sync.data.bABS = false;
    sync.data.fSteeringLock = 30.0f;
    sync.data.fTractionLoss = 0.85f;
    sync.data.fTractionBias = 0.48f;
    sync.data.fSuspensionForceLevel = 1.2f;
    sync.data.fSuspensionDamping = 0.1f;
    sync.data.fSuspensionHighSpdDamping = 0.0f;
    sync.data.fSuspensionUpperLimit = 0.28f;
    sync.data.fSuspensionLowerLimit = -0.14f;
    sync.data.fSuspensionFrontRearBias = 0.5f;
    sync.data.fSuspensionAntiDiveMultiplier = 0.3f;
    sync.data.fCollisionDamageMultiplier = 0.5f;
    sync.data.uiModelFlags = 0x00002000;
    sync.data.uiHandlingFlags = 0x01000000;
    sync.data.fSeatOffsetDistance = 0.26f;
    sync.data.ucAnimGroup = 0;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehicleHandlingSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_FLOAT_EQ(1500.0f, out.data.fMass);
    EXPECT_FLOAT_EQ(4000.0f, out.data.fTurnMass);
    EXPECT_FLOAT_EQ(2.5f, out.data.fDragCoeff);
    EXPECT_EQ(75, out.data.ucPercentSubmerged);
    EXPECT_EQ('R', out.data.ucDriveType);
    EXPECT_EQ(5, out.data.ucNumberOfGears);
    EXPECT_FLOAT_EQ(200.0f, out.data.fMaxVelocity);
    EXPECT_FALSE(out.data.bABS);
    EXPECT_FLOAT_EQ(30.0f, out.data.fSteeringLock);
    EXPECT_EQ(0x00002000u, out.data.uiModelFlags);
    EXPECT_EQ(0x01000000u, out.data.uiHandlingFlags);
    EXPECT_EQ(0, out.data.ucAnimGroup);
}

// ============================================================================
// Unoccupied vehicle sync (composite - uses many sub-syncs)
// ============================================================================

// SUnoccupiedVehicleSync: composite structure with flag bits controlling
// which sub-fields are present. This test enables position + health only.
// Position uses quantized SPositionSync; health uses SVehicleHealthSync.
TEST(SUnoccupiedVehicleSync, RoundTrip_PositionAndHealth)
{
    MockBitStream          bs;
    SUnoccupiedVehicleSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.vehicleID = ElementID(100);
    sync.data.ucTimeContext = 42;
    sync.data.bSyncPosition = true;
    sync.data.bSyncHealth = true;
    sync.data.vecPosition = CVector(100.0f, 200.0f, 10.0f);
    sync.data.fHealth = 1000.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SUnoccupiedVehicleSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(42, out.data.ucTimeContext);
    EXPECT_TRUE(out.data.bSyncPosition);
    EXPECT_TRUE(out.data.bSyncHealth);
    EXPECT_FALSE(out.data.bSyncRotation);
    EXPECT_NEAR(100.0f, out.data.vecPosition.fX, 0.01f);
    EXPECT_NEAR(200.0f, out.data.vecPosition.fY, 0.01f);
    EXPECT_FLOAT_EQ(10.0f, out.data.vecPosition.fZ);
    EXPECT_NEAR(1000.0f, out.data.fHealth, 0.6f);
}

// SUnoccupiedVehicleSync with velocity enabled: exercises the SVelocitySync
// sub-structure which uses NormVector encoding for the direction component.
// The simpler PositionAndHealth test above doesn't cover this path.
TEST(SUnoccupiedVehicleSync, RoundTrip_WithVelocity)
{
    MockBitStream          bs;
    SUnoccupiedVehicleSync sync;
    std::memset(&sync.data, 0, sizeof(sync.data));
    sync.data.vehicleID = ElementID(200);
    sync.data.ucTimeContext = 7;
    sync.data.bSyncPosition = true;
    sync.data.bSyncVelocity = true;
    sync.data.bSyncHealth = true;
    sync.data.vecPosition = CVector(500.0f, -300.0f, 25.0f);
    sync.data.vecVelocity = CVector(5.0f, 10.0f, -2.0f);
    sync.data.fHealth = 800.0f;
    sync.Write(bs);
    bs.ResetReadPointer();
    SUnoccupiedVehicleSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(7, out.data.ucTimeContext);
    EXPECT_TRUE(out.data.bSyncPosition);
    EXPECT_TRUE(out.data.bSyncVelocity);
    EXPECT_TRUE(out.data.bSyncHealth);
    EXPECT_FALSE(out.data.bSyncRotation);
    EXPECT_NEAR(500.0f, out.data.vecPosition.fX, 0.01f);
    EXPECT_NEAR(-300.0f, out.data.vecPosition.fY, 0.01f);
    // Velocity uses NormVector direction encoding, so allow some tolerance
    EXPECT_NEAR(5.0f, out.data.vecVelocity.fX, 0.05f);
    EXPECT_NEAR(10.0f, out.data.vecVelocity.fY, 0.05f);
    EXPECT_NEAR(-2.0f, out.data.vecVelocity.fZ, 0.05f);
    EXPECT_NEAR(800.0f, out.data.fHealth, 0.6f);
}

// ============================================================================
// Unoccupied push sync
// ============================================================================

// Lightweight sync for pushing unoccupied vehicles - only carries the
// vehicle's ElementID.
TEST(SUnoccupiedPushSync, RoundTrip)
{
    MockBitStream       bs;
    SUnoccupiedPushSync sync;
    sync.data.vehicleID = ElementID(999);
    sync.Write(bs);
    bs.ResetReadPointer();
    SUnoccupiedPushSync out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_EQ(999u, out.data.vehicleID.Value());
}

// ============================================================================
// SVehicleDamageSyncMethodeB (composite)
// ============================================================================

// Composite structure that selectively syncs doors, wheels, panels, and
// lights damage. Each sub-component uses SVehiclePartStateSyncMethodeB.
// IMPORTANT: Cannot memset data because it contains objects with vtables
// (SVehiclePartStateSyncMethodeB inherits from ISyncStructure).
// Zeroing the vtable pointer would cause access violations.
TEST(SVehicleDamageSyncMethodeB, RoundTrip_DoorsAndWheels)
{
    MockBitStream              bs;
    SVehicleDamageSyncMethodeB sync;
    // Set which sub-components to sync
    sync.data.bSyncDoors = true;
    sync.data.bSyncWheels = true;
    sync.data.bSyncPanels = false;
    sync.data.bSyncLights = false;
    // Set door states (6 doors, 3 bits each → [0,7])
    sync.data.doors.data.ucStates[0] = 3;
    sync.data.doors.data.ucStates[1] = 0;
    sync.data.doors.data.ucStates[2] = 7;
    sync.data.doors.data.ucStates[3] = 1;
    sync.data.doors.data.ucStates[4] = 0;
    sync.data.doors.data.ucStates[5] = 5;
    // Set wheel states (4 wheels, 2 bits each → [0,3])
    sync.data.wheels.data.ucStates[0] = 2;
    sync.data.wheels.data.ucStates[1] = 0;
    sync.data.wheels.data.ucStates[2] = 3;
    sync.data.wheels.data.ucStates[3] = 1;
    sync.Write(bs);
    bs.ResetReadPointer();
    SVehicleDamageSyncMethodeB out;
    EXPECT_TRUE(out.Read(bs));
    EXPECT_TRUE(out.data.bSyncDoors);
    EXPECT_TRUE(out.data.bSyncWheels);
    EXPECT_FALSE(out.data.bSyncPanels);
    EXPECT_FALSE(out.data.bSyncLights);
    EXPECT_EQ(3, out.data.doors.data.ucStates[0]);
    EXPECT_EQ(7, out.data.doors.data.ucStates[2]);
    EXPECT_EQ(2, out.data.wheels.data.ucStates[0]);
    EXPECT_EQ(3, out.data.wheels.data.ucStates[2]);
}
