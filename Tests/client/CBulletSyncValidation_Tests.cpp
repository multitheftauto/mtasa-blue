/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Tests/client/CBulletSyncValidation_Tests.cpp
 *  PURPOSE:     Google Test suite for the shared bullet sync validation
 *
 *  Covers the bounds that keep crafted bullet sync payloads away from the
 *  receiving client's line of sight processing: position sanity, trajectory
 *  length against weapon range, muzzle distance from the shooter and the
 *  optional damage payload.
 *
 *****************************************************************************/

#include <gtest/gtest.h>
#include <SharedUtil.Defines.h>
#include <CBulletSyncValidation.h>
#include <limits>

namespace
{
    constexpr float kNaN = std::numeric_limits<float>::quiet_NaN();
    constexpr float kInf = std::numeric_limits<float>::infinity();

    // Windows defines INFINITE as 0xFFFFFFFF. Assigned to a float it becomes a huge but
    // finite value, which is why NaN/infinity checks alone never caught this payload.
    constexpr float kHugeFinite = static_cast<float>(0xFFFFFFFFu);

    // Typical sniper rifle range
    constexpr float kWeaponRange = 100.0f;

    // Colt 45 / Silenced / Deagle at poor skill - the shortest range in the bullet sync set
    constexpr float kHandgunRange = 30.0f;

    constexpr float ExpectedLimit(float range)
    {
        return range * BulletSync::RANGE_TOLERANCE + BulletSync::RANGE_SLACK;
    }
}  // namespace

///////////////////////////////////////////////////////////////
//
// IsSanePosition
//
///////////////////////////////////////////////////////////////

TEST(BulletSyncValidation, PositionAcceptsOrdinaryCoordinates)
{
    EXPECT_TRUE(BulletSync::IsSanePosition(CVector(0.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(BulletSync::IsSanePosition(CVector(1500.0f, -2200.0f, 25.0f)));
}

TEST(BulletSyncValidation, PositionRejectsNaNAndInfinity)
{
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(kNaN, 0.0f, 0.0f)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(0.0f, kNaN, 0.0f)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(0.0f, 0.0f, kNaN)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(kInf, 0.0f, 0.0f)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(0.0f, -kInf, 0.0f)));
}

TEST(BulletSyncValidation, PositionRejectsHugeFiniteCoordinates)
{
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(kHugeFinite, kHugeFinite, 25.0f)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(9999.0f, 9999.0f, 999999.0f)));
}

TEST(BulletSyncValidation, PositionBoundIsInclusive)
{
    const float bound = BulletSync::MAX_WORLD_COORD;
    EXPECT_TRUE(BulletSync::IsSanePosition(CVector(bound, -bound, bound)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(bound + 1.0f, 0.0f, 0.0f)));
    EXPECT_FALSE(BulletSync::IsSanePosition(CVector(0.0f, -bound - 1.0f, 0.0f)));
}

///////////////////////////////////////////////////////////////
//
// ValidateTrajectory
//
///////////////////////////////////////////////////////////////

TEST(BulletSyncValidation, TrajectoryAcceptsShotWithinWeaponRange)
{
    const CVector start(100.0f, 100.0f, 10.0f);
    const CVector end(160.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
}

TEST(BulletSyncValidation, TrajectoryRejectsZeroLength)
{
    const CVector point(100.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooShort, BulletSync::ValidateTrajectory(point, point, kWeaponRange));
}

TEST(BulletSyncValidation, TrajectoryRejectsShotBeyondWeaponRange)
{
    const CVector start(100.0f, 100.0f, 10.0f);
    const CVector end(100.0f + ExpectedLimit(kWeaponRange) + 1.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
}

TEST(BulletSyncValidation, TrajectoryAllowsRangeTolerance)
{
    const CVector start(100.0f, 100.0f, 10.0f);
    const CVector end(100.0f + ExpectedLimit(kWeaponRange) - 0.5f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
}

// Regression for #5122. The packet reports the gun muzzle as the start while the game traced the
// shot from the camera, so a handgun fired at its nominal range measures noticeably longer than
// that range. Scaling the tolerance alone leaves 3m of headroom on a 30m weapon, which dropped
// ordinary handgun shots and took onPlayerWeaponFire down with them.
TEST(BulletSyncValidation, TrajectoryAcceptsShortRangeWeaponWithCameraOffset)
{
    const CVector start(100.0f, 100.0f, 10.0f);

    for (const float length : {30.0f, 35.0f, 40.0f})
    {
        const CVector end(100.0f + length, 100.0f, 10.0f);
        EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateTrajectory(start, end, kHandgunRange)) << "handgun shot of " << length << "m";
    }
}

// The slack is a fixed allowance, not an open door for short range weapons
TEST(BulletSyncValidation, TrajectoryStillRejectsShortRangeWeaponAtRifleDistance)
{
    const CVector start(100.0f, 100.0f, 10.0f);
    const CVector end(170.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, end, kHandgunRange));
}

// Long range weapons gain the same absolute slack, not a proportional one
TEST(BulletSyncValidation, TrajectorySlackDoesNotScaleWithRange)
{
    constexpr float sniperRange = 300.0f;
    const CVector   start(100.0f, 100.0f, 10.0f);

    const CVector withinLimit(100.0f + ExpectedLimit(sniperRange) - 1.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateTrajectory(start, withinLimit, sniperRange));

    const CVector beyondLimit(100.0f + ExpectedLimit(sniperRange) + 1.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, beyondLimit, sniperRange));
}

// A high bullet: end far above the muzzle must fail on length, not slip through on height
TEST(BulletSyncValidation, TrajectoryRejectsExcessiveHeight)
{
    const CVector start(100.0f, 100.0f, 10.0f);
    const CVector end(100.0f, 100.0f, 3000.0f);
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
}

TEST(BulletSyncValidation, TrajectoryFallsBackToHardCapWhenRangeUnknown)
{
    const CVector start(100.0f, 100.0f, 10.0f);

    const CVector withinCap(100.0f + BulletSync::MAX_TRAJECTORY_LENGTH - 1.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateTrajectory(start, withinCap, 0.0f));

    const CVector beyondCap(100.0f + BulletSync::MAX_TRAJECTORY_LENGTH + 1.0f, 100.0f, 10.0f);
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, beyondCap, 0.0f));
}

// A hostile range value must never widen the accepted envelope
TEST(BulletSyncValidation, TrajectoryIgnoresUnusableRangeValues)
{
    const CVector start(100.0f, 100.0f, 10.0f);
    const CVector beyondCap(100.0f + BulletSync::MAX_TRAJECTORY_LENGTH + 1.0f, 100.0f, 10.0f);

    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, beyondCap, kNaN));
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, beyondCap, kInf));
    EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, beyondCap, -5.0f));
}

TEST(BulletSyncValidation, TrajectoryReportsWhichEndIsOutOfBounds)
{
    const CVector sane(100.0f, 100.0f, 10.0f);
    const CVector insane(kHugeFinite, kHugeFinite, 10.0f);

    EXPECT_EQ(BulletSync::EResult::BadStart, BulletSync::ValidateTrajectory(insane, sane, kWeaponRange));
    EXPECT_EQ(BulletSync::EResult::BadEnd, BulletSync::ValidateTrajectory(sane, insane, kWeaponRange));
}

///////////////////////////////////////////////////////////////
//
// ValidateMuzzleOrigin
//
///////////////////////////////////////////////////////////////

TEST(BulletSyncValidation, MuzzleAcceptsPositionNextToShooter)
{
    const CVector shooter(500.0f, 500.0f, 20.0f);
    const CVector muzzle(500.5f, 500.0f, 21.0f);
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateMuzzleOrigin(shooter, muzzle, false));
}

TEST(BulletSyncValidation, MuzzleRejectsPositionFarFromShooter)
{
    const CVector shooter(500.0f, 500.0f, 20.0f);
    const CVector muzzle(500.0f + BulletSync::MAX_MUZZLE_OFFSET_ON_FOOT + 1.0f, 500.0f, 20.0f);
    EXPECT_EQ(BulletSync::EResult::MuzzleTooFarFromShooter, BulletSync::ValidateMuzzleOrigin(shooter, muzzle, false));
}

TEST(BulletSyncValidation, MuzzleAllowsLargerOffsetInVehicle)
{
    const CVector shooter(500.0f, 500.0f, 20.0f);
    const CVector muzzle(500.0f + BulletSync::MAX_MUZZLE_OFFSET_ON_FOOT + 10.0f, 500.0f, 20.0f);

    EXPECT_EQ(BulletSync::EResult::MuzzleTooFarFromShooter, BulletSync::ValidateMuzzleOrigin(shooter, muzzle, false));
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateMuzzleOrigin(shooter, muzzle, true));
}

TEST(BulletSyncValidation, MuzzleRejectsOutOfBoundsRegardlessOfShooter)
{
    const CVector shooter(500.0f, 500.0f, 20.0f);
    const CVector muzzle(kHugeFinite, kHugeFinite, 20.0f);
    EXPECT_EQ(BulletSync::EResult::BadStart, BulletSync::ValidateMuzzleOrigin(shooter, muzzle, false));
}

// An unusable shooter position must not turn into an accidental rejection
TEST(BulletSyncValidation, MuzzleSkipsComparisonWhenShooterPositionIsUnusable)
{
    const CVector muzzle(500.0f, 500.0f, 20.0f);
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateMuzzleOrigin(CVector(kNaN, kNaN, kNaN), muzzle, false));
}

///////////////////////////////////////////////////////////////
//
// ValidateDamagePayload
//
///////////////////////////////////////////////////////////////

TEST(BulletSyncValidation, DamagePayloadAcceptsOrdinaryValues)
{
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateDamagePayload(0.0f, 0));
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateDamagePayload(46.2f, 3));
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateDamagePayload(BulletSync::MAX_DAMAGE, BulletSync::MAX_BODY_ZONE));
}

TEST(BulletSyncValidation, DamagePayloadRejectsOutOfRangeDamage)
{
    EXPECT_EQ(BulletSync::EResult::BadDamage, BulletSync::ValidateDamagePayload(-0.5f, 0));
    EXPECT_EQ(BulletSync::EResult::BadDamage, BulletSync::ValidateDamagePayload(BulletSync::MAX_DAMAGE + 1.0f, 0));
    EXPECT_EQ(BulletSync::EResult::BadDamage, BulletSync::ValidateDamagePayload(kHugeFinite, 0));
}

TEST(BulletSyncValidation, DamagePayloadRejectsNaNAndInfinity)
{
    EXPECT_EQ(BulletSync::EResult::BadDamage, BulletSync::ValidateDamagePayload(kNaN, 0));
    EXPECT_EQ(BulletSync::EResult::BadDamage, BulletSync::ValidateDamagePayload(kInf, 0));
}

TEST(BulletSyncValidation, DamagePayloadRejectsOutOfRangeHitZone)
{
    EXPECT_EQ(BulletSync::EResult::BadHitZone, BulletSync::ValidateDamagePayload(10.0f, BulletSync::MAX_BODY_ZONE + 1));
    EXPECT_EQ(BulletSync::EResult::BadHitZone, BulletSync::ValidateDamagePayload(10.0f, 255));
}

///////////////////////////////////////////////////////////////
//
// Regression: payloads observed in the wild
//
///////////////////////////////////////////////////////////////

// Crafted shot fired from 0xFFFFFFFF-as-float towards a target's head bone. The huge but
// finite start position is what CVector::IsValid lets through.
TEST(BulletSyncValidation, RejectsCraftedMuzzleAtHugeFiniteCoordinate)
{
    const CVector shooter(1500.0f, 1500.0f, 25.0f);
    const CVector start(kHugeFinite, kHugeFinite, 25.0f);
    const CVector end(1520.0f, 1500.0f, 26.0f);

    EXPECT_TRUE(start.IsValid());  // the check this payload was built to pass

    EXPECT_EQ(BulletSync::EResult::BadStart, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
    EXPECT_EQ(BulletSync::EResult::BadStart, BulletSync::ValidateMuzzleOrigin(shooter, start, false));
}

// Same idea with plausible looking world coordinates instead of an extreme value
TEST(BulletSyncValidation, RejectsCraftedMuzzleAtArbitraryWorldCoordinate)
{
    const CVector shooter(1500.0f, 1500.0f, 25.0f);
    const CVector end(1520.0f, 1500.0f, 26.0f);

    for (const float coord : {2000.0f, 5000.0f, 9999.0f})
    {
        const CVector start(coord, coord, 25.0f);
        EXPECT_TRUE(BulletSync::IsSanePosition(start)) << "coord " << coord << " is inside the world box";
        EXPECT_EQ(BulletSync::EResult::MuzzleTooFarFromShooter, BulletSync::ValidateMuzzleOrigin(shooter, start, false))
            << "coord " << coord << " should be rejected relative to the shooter";
        EXPECT_EQ(BulletSync::EResult::TrajectoryTooLong, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
    }
}

// A legitimate shot from the same distant area must still be accepted
TEST(BulletSyncValidation, AcceptsLegitimateShotFarFromWorldCentre)
{
    const CVector shooter(2000.0f, 2000.0f, 25.0f);
    const CVector start(2000.4f, 2000.0f, 26.0f);
    const CVector end(2040.0f, 2000.0f, 26.0f);

    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateMuzzleOrigin(shooter, start, false));
    EXPECT_EQ(BulletSync::EResult::Valid, BulletSync::ValidateTrajectory(start, end, kWeaponRange));
}
