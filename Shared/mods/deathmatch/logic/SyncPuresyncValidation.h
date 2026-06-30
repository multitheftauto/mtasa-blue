/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/SyncPuresyncValidation.h
 *  PURPOSE:     Server-side on-foot puresync movement validation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifndef MTA_CLIENT

    #include "Utils.h"

// Anti-cheat: validate a movement velocity coming from a client puresync packet.
//
// Returns false when the velocity is non-finite (NaN/Inf) or its magnitude exceeds iMaxSpeedKmh,
// meaning the packet has been tampered with (e.g. cheats setting velocity magnitude 100 GTA-units
// to launch nearby vehicles). The caller drops the whole packet so neither the bogus velocity nor
// the position bundled in the same packet is applied or relayed to other players.
//
// iMaxSpeedKmh <= 0 disables the magnitude check. 1 GTA-unit of velocity equals 180 km/h
// (see getElementVelocity wiki), so the comparison is done in GTA-units to avoid a per-packet sqrt.
inline bool IsSyncedVelocityAcceptable(const CVector& vecVelocity, int iMaxSpeedKmh)
{
    if (!vecVelocity.IsValid())
        return false;

    if (iMaxSpeedKmh <= 0)
        return true;

    const float fMaxVelocity = static_cast<float>(iMaxSpeedKmh) / 180.0f;
    return vecVelocity.LengthSquared() <= fMaxVelocity * fMaxVelocity;
}

// Anti-cheat: validate that an on-foot player could not have moved vecLast->vecNew in ullElapsedMs.
// Cheats forge position in outgoing puresync without moving locally; this catches position-only
// launches even when the velocity field is omitted from the packet.
inline bool IsSyncedPositionDeltaAcceptable(const CVector& vecLastPosition, const CVector& vecNewPosition, unsigned long long ullElapsedMs,
                                            int iMaxSpeedKmh, float fMarginMultiplier = 1.35f)
{
    if (iMaxSpeedKmh <= 0)
        return true;

    if (!vecLastPosition.IsValid() || !vecNewPosition.IsValid())
        return false;

    if (ullElapsedMs == 0)
        ullElapsedMs = 1;

    const float fDistance = DistanceBetweenPoints3D(vecLastPosition, vecNewPosition);
    const float fMaxSpeedMs = static_cast<float>(iMaxSpeedKmh) / 3.6f;
    const float fMaxDistance = fMaxSpeedMs * (static_cast<float>(ullElapsedMs) / 1000.0f) * fMarginMultiplier;
    return fDistance <= fMaxDistance;
}

// Drop forged on-foot puresync before it is applied or relayed. When the player has valid vehicle
// contact, callers pass contact-relative coordinates for vecReferencePosition/vecNewPosition so fast
// vehicle surf does not false-positive against absolute on-foot speed limits.
inline bool IsPlayerPuresyncMovementAcceptable(const CVector& vecReferencePosition, unsigned long long ullElapsedMs, int iSyncIntervalMs,
                                               const CVector& vecNewPosition, bool bSyncingVelocity, const CVector& vecVelocity, int iMaxSpeedKmh,
                                               bool bServerAuthorizedTeleport)
{
    if (iMaxSpeedKmh <= 0 || bServerAuthorizedTeleport)
        return true;

    if (ullElapsedMs == 0)
        ullElapsedMs = static_cast<unsigned long long>(iSyncIntervalMs > 0 ? iSyncIntervalMs : 1);

    if (!IsSyncedPositionDeltaAcceptable(vecReferencePosition, vecNewPosition, ullElapsedMs, iMaxSpeedKmh))
        return false;

    if (bSyncingVelocity && !IsSyncedVelocityAcceptable(vecVelocity, iMaxSpeedKmh))
        return false;

    return true;
}

// Resolve which coordinates to feed into IsPlayerPuresyncMovementAcceptable.
inline void ResolvePuresyncMovementDelta(const bool bVehicleContact, const ElementID contactElementId, const ElementID lastAcceptedContactElementId,
                                         const CVector& vecNewRelative, const CVector& vecNewAbsolute, const CVector& vecLastAcceptedContactRelative,
                                         const CVector& vecLastAcceptedAbsolute, const CVector& vecServerContactRelative, CElement* pServerContactElement,
                                         CElement* pContactElement, const bool bHasLastAcceptedAbsolute, const CVector& vecServerAbsolutePosition,
                                         CVector& outReferencePosition, CVector& outNewPosition)
{
    if (bVehicleContact)
    {
        outNewPosition = vecNewRelative;
        if (lastAcceptedContactElementId == contactElementId)
            outReferencePosition = vecLastAcceptedContactRelative;
        else if (pServerContactElement == pContactElement)
            outReferencePosition = vecServerContactRelative;
        else
            outReferencePosition = vecNewRelative;
    }
    else
    {
        outNewPosition = vecNewAbsolute;
        outReferencePosition = bHasLastAcceptedAbsolute ? vecLastAcceptedAbsolute : vecServerAbsolutePosition;
    }
}

#endif
