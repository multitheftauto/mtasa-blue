/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/SyncBulletsyncValidation.h
 *  PURPOSE:     Player-relative bullet sync and weapon-aim validation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CCommon.h"
#include "CVector.h"
#include <cmath>

namespace SyncBulletsyncValidation
{
    // Degenerate segment guard (SetTargetTarget divides by origin-target distance on clients).
    inline constexpr float MIN_TRAJECTORY_DISTANCE_SQ = 0.0001f;

    // Physical muzzle/mount offsets from the ped or seated sync point — not gameplay caps.
    inline constexpr float PED_WEAPON_ORIGIN_MAX_OFFSET = 5.0f;
    inline constexpr float VEHICLE_WEAPON_ORIGIN_MAX_OFFSET = 25.0f;

    inline constexpr float              MAX_DAMAGE = 200.0f;
    inline constexpr unsigned char      MAX_BODY_ZONE = 9;
    inline constexpr float              WEAPON_RANGE_TOLERANCE = 1.15f;
    inline constexpr unsigned int       MAX_PACKETS_PER_SECOND = 25;
    inline constexpr unsigned long long RATE_WINDOW_MS = 1000;

    inline float DistanceSquared3D(const CVector& vecA, const CVector& vecB) noexcept
    {
        const float fDx = vecA.fX - vecB.fX;
        const float fDy = vecA.fY - vecB.fY;
        const float fDz = vecA.fZ - vecB.fZ;
        return (fDx * fDx) + (fDy * fDy) + (fDz * fDz);
    }

    inline bool IsNaN(float fValue) noexcept
    {
        return fValue != fValue;
    }

    inline bool IsSyncedBulletVectorAcceptable(const CVector& vec) noexcept
    {
        return vec.IsValid();
    }

    inline float GetMaxBulletOriginOffsetFromPlayer(bool bInVehicle) noexcept
    {
        return bInVehicle ? VEHICLE_WEAPON_ORIGIN_MAX_OFFSET : PED_WEAPON_ORIGIN_MAX_OFFSET;
    }

    inline float GetMaxBulletReachFromPlayer(bool bInVehicle, float fWeaponRange) noexcept
    {
        const float fRange = fWeaponRange > 0.0f ? fWeaponRange * WEAPON_RANGE_TOLERANCE : 0.0f;
        return GetMaxBulletOriginOffsetFromPlayer(bInVehicle) + fRange;
    }

    inline bool IsSyncedBulletSegmentNonDegenerate(const CVector& vecStart, const CVector& vecEnd) noexcept
    {
        if (!IsSyncedBulletVectorAcceptable(vecStart) || !IsSyncedBulletVectorAcceptable(vecEnd))
            return false;

        const float fMovementSq = DistanceSquared3D(vecStart, vecEnd);

        if (IsNaN(fMovementSq))
            return false;

        return fMovementSq >= MIN_TRAJECTORY_DISTANCE_SQ;
    }

    inline bool IsSyncedBulletOriginPlausible(const CVector& vecOrigin, const CVector& vecPlayerPosition, bool bInVehicle) noexcept
    {
        if (!IsSyncedBulletVectorAcceptable(vecOrigin) || !IsSyncedBulletVectorAcceptable(vecPlayerPosition))
            return false;

        const float fMaxOffset = GetMaxBulletOriginOffsetFromPlayer(bInVehicle);
        return DistanceSquared3D(vecOrigin, vecPlayerPosition) <= (fMaxOffset * fMaxOffset);
    }

    inline bool IsSyncedBulletImpactPlausible(const CVector& vecImpact, const CVector& vecPlayerPosition, bool bInVehicle, float fWeaponRange) noexcept
    {
        if (!IsSyncedBulletVectorAcceptable(vecImpact) || !IsSyncedBulletVectorAcceptable(vecPlayerPosition))
            return false;

        const float fMaxReach = GetMaxBulletReachFromPlayer(bInVehicle, fWeaponRange);
        return DistanceSquared3D(vecImpact, vecPlayerPosition) <= (fMaxReach * fMaxReach);
    }

    inline bool IsSyncedBulletSegmentWithinWeaponRange(const CVector& vecStart, const CVector& vecEnd, float fWeaponRange) noexcept
    {
        if (!IsSyncedBulletSegmentNonDegenerate(vecStart, vecEnd))
            return false;

        if (fWeaponRange <= 0.0f)
            return true;

        const float fMaxRange = fWeaponRange * WEAPON_RANGE_TOLERANCE;
        return DistanceSquared3D(vecStart, vecEnd) <= (fMaxRange * fMaxRange);
    }

    inline bool IsSyncedBulletsyncGeometryAcceptable(const CVector& vecPlayerPosition, bool bInVehicle, const CVector& vecStart, const CVector& vecEnd,
                                                     float fWeaponRange) noexcept
    {
        if (!IsSyncedBulletOriginPlausible(vecStart, vecPlayerPosition, bInVehicle))
            return false;

        if (!IsSyncedBulletSegmentWithinWeaponRange(vecStart, vecEnd, fWeaponRange))
            return false;

        if (!IsSyncedBulletImpactPlausible(vecEnd, vecPlayerPosition, bInVehicle, fWeaponRange))
            return false;

        return true;
    }

    inline bool IsSyncedBulletDamageAcceptable(float fDamage, unsigned char ucZone, ElementID damagedElementId) noexcept
    {
        if (IsNaN(fDamage))
            return false;

        if (fDamage <= 0.0f)
            return true;

        if (fDamage > MAX_DAMAGE)
            return false;

        if (ucZone > MAX_BODY_ZONE)
            return false;

        if (damagedElementId == 0)
            return false;

        return true;
    }

    inline bool IsSyncedWeaponAimAcceptable(const CVector& vecPlayerPosition, const CVector& vecOrigin, const CVector& vecTarget, float fWeaponRange,
                                            bool bInVehicle) noexcept
    {
        return IsSyncedBulletsyncGeometryAcceptable(vecPlayerPosition, bInVehicle, vecOrigin, vecTarget, fWeaponRange);
    }

    inline bool IsSyncedBulletsyncPacketAcceptable(const CVector& vecPlayerPosition, bool bInVehicle, const CVector& vecStart, const CVector& vecEnd,
                                                   float fDamage, unsigned char ucZone, ElementID damagedElementId, float fWeaponRange) noexcept
    {
        if (!IsSyncedBulletsyncGeometryAcceptable(vecPlayerPosition, bInVehicle, vecStart, vecEnd, fWeaponRange))
            return false;

        if (!IsSyncedBulletDamageAcceptable(fDamage, ucZone, damagedElementId))
            return false;

        return true;
    }

    inline bool IsSyncedBulletTrajectoryAcceptable(const CVector& vecStart, const CVector& vecEnd) noexcept
    {
        return IsSyncedBulletSegmentNonDegenerate(vecStart, vecEnd);
    }

    inline bool IsSyncedBulletOriginNearPlayer(const CVector& vecStart, const CVector& vecPlayerPosition, bool bInVehicle) noexcept
    {
        return IsSyncedBulletOriginPlausible(vecStart, vecPlayerPosition, bInVehicle);
    }

    inline bool IsSyncedBulletWithinWeaponRange(const CVector& vecStart, const CVector& vecEnd, float fWeaponRange) noexcept
    {
        return IsSyncedBulletSegmentWithinWeaponRange(vecStart, vecEnd, fWeaponRange);
    }
}
