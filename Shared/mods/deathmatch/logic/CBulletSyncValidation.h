/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/deathmatch/logic/CBulletSyncValidation.h
 *  PURPOSE:     Shared sanity checks for bullet sync trajectories
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <CVector.h>

// Bullet sync carries raw floats that end up in the receiving client's line of sight
// processing, where out of range coordinates overflow the world sector indices and take
// the game down. Note that a huge but finite coordinate (e.g. 0xFFFFFFFF reinterpreted as
// a float) passes CVector::IsValid, so magnitude has to be bounded explicitly.
//
// Both sides must reach the same verdict for the same data, so every check lives here.
namespace BulletSync
{
    // Loose sanity net. The relative checks below keep legitimate shots far inside this.
    inline constexpr float MAX_WORLD_COORD = 20000.0f;

    inline constexpr float MIN_TRAJECTORY_LENGTH = 0.01f;
    inline constexpr float MAX_TRAJECTORY_LENGTH = 500.0f;

    // Skill stats and network timing make the reported length drift from the nominal range.
    inline constexpr float RANGE_TOLERANCE = 1.1f;

    // The reported start is the gun muzzle, but the game traces the shot from the camera, so the
    // measured length picks up the third person camera offset. That offset does not shrink with
    // the weapon range: a proportional tolerance alone leaves a 30m pistol just 3m of headroom
    // and drops ordinary shots, while a rifle never notices. Slack has to be absolute.
    inline constexpr float RANGE_SLACK = 15.0f;

    // Distance between the shooter and the muzzle position they report. Vehicle weapons
    // have large offsets (Hunter ~5m) on top of vehicle size and lag compensation.
    inline constexpr float MAX_MUZZLE_OFFSET_ON_FOOT = 50.0f;
    inline constexpr float MAX_MUZZLE_OFFSET_IN_VEHICLE = 100.0f;

    inline constexpr float        MAX_DAMAGE = 200.0f;
    inline constexpr std::uint8_t MAX_BODY_ZONE = 9;

    enum class EResult : std::uint8_t
    {
        Valid,
        BadStart,
        BadEnd,
        TrajectoryTooShort,
        TrajectoryTooLong,
        MuzzleTooFarFromShooter,
        BadDamage,
        BadHitZone,
    };

    // NaN and infinity fail the comparison as well, so no separate finite check is needed.
    inline bool IsSanePosition(const CVector& position) noexcept
    {
        return std::fabs(position.fX) <= MAX_WORLD_COORD && std::fabs(position.fY) <= MAX_WORLD_COORD && std::fabs(position.fZ) <= MAX_WORLD_COORD;
    }

    // Only safe to call once both vectors passed IsSanePosition, otherwise the squares overflow.
    inline float GetDistanceSq(const CVector& from, const CVector& to) noexcept
    {
        const float dx = to.fX - from.fX;
        const float dy = to.fY - from.fY;
        const float dz = to.fZ - from.fZ;

        return (dx * dx) + (dy * dy) + (dz * dz);
    }

    // Pass a non positive maxRange when the weapon range is unknown to fall back to the hard cap.
    // NaN and infinity are handled by the same path: neither can widen the accepted envelope.
    inline EResult ValidateTrajectory(const CVector& start, const CVector& end, float maxRange) noexcept
    {
        if (!IsSanePosition(start))
            return EResult::BadStart;

        if (!IsSanePosition(end))
            return EResult::BadEnd;

        const float lengthSq = GetDistanceSq(start, end);

        if (lengthSq < MIN_TRAJECTORY_LENGTH * MIN_TRAJECTORY_LENGTH)
            return EResult::TrajectoryTooShort;

        float limit = MAX_TRAJECTORY_LENGTH;
        if (maxRange > 0.0f)
            limit = std::min(maxRange * RANGE_TOLERANCE + RANGE_SLACK, MAX_TRAJECTORY_LENGTH);

        if (lengthSq > limit * limit)
            return EResult::TrajectoryTooLong;

        return EResult::Valid;
    }

    inline EResult ValidateMuzzleOrigin(const CVector& shooter, const CVector& start, bool isInVehicle) noexcept
    {
        if (!IsSanePosition(start))
            return EResult::BadStart;

        // Nothing to compare against, the trajectory checks still apply.
        if (!IsSanePosition(shooter))
            return EResult::Valid;

        const float limit = isInVehicle ? MAX_MUZZLE_OFFSET_IN_VEHICLE : MAX_MUZZLE_OFFSET_ON_FOOT;

        if (GetDistanceSq(shooter, start) > limit * limit)
            return EResult::MuzzleTooFarFromShooter;

        return EResult::Valid;
    }

    inline EResult ValidateDamagePayload(float damage, std::uint8_t zone) noexcept
    {
        // Negated range test so NaN fails it too, without relying on isfinite surviving the optimiser
        if (!(damage >= 0.0f && damage <= MAX_DAMAGE))
            return EResult::BadDamage;

        if (zone > MAX_BODY_ZONE)
            return EResult::BadHitZone;

        return EResult::Valid;
    }
}  // namespace BulletSync
