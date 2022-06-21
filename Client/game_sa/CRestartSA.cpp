/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRestartSA.cpp
 *  PURPOSE:     Restart points
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

/**
 * Prevent the next restart from being overridden and instead use the closest police or hospital restart point
 */
VOID CRestartSA::CancelOverrideRestart()
{
    DEBUG_TRACE("VOID CRestartSA::CancelOverrideRestart (  )");
    MemPutFast<BYTE>(VAR_OverrideNextRestart, 0);
}

/**
 * Override the next restart, so that the player restarts at a specific position instead of the nearest
 * police or hospital restart point
 * @param vecPosition CVector * containing the position to restart at
 * @param fRotation Rotation in radians to face once restarted
 */
VOID CRestartSA::OverrideNextRestart(CVector* vecPosition, FLOAT fRotation)
{
    DEBUG_TRACE("VOID CRestartSA::OverrideNextRestart ( CVector * vecPosition, FLOAT fRotation )");
    MemPutFast<BYTE>(VAR_OverrideNextRestart, 1);
    MemCpyFast((void*)VAR_OverrideNextRestartPosition, vecPosition, sizeof(CVector));
    MemPut<FLOAT>(VAR_OverrideNextRestartRotation, fRotation);
}

/**
 * Find the closest police restart point to a specific point
 * @param vecClosestTo CVector * containing the position you want to find the closest restart point to
 * @param vecClosestRestartPoint CVector * that returns the position of the closest restart point
 * @param fRotation FLOAT * that returns the rotation at which the restart point restarts the player
 */
VOID CRestartSA::FindClosestPoliceRestartPoint(CVector* vecClosestTo, CVector* vecClosestRestartPoint, FLOAT* fRotation)
{
    DEBUG_TRACE("VOID CRestartSA::FindClosestPoliceRestartPoint ( CVector * vecClosestTo, CVector * vecClosestRestartPoint, FLOAT * fRotation )");

    // CRestart::FindClosestPoliceRestartPoint
    ((void(__cdecl*)(CVector, CVector*, float*))FUNC_FindClosestPoliceRestartPoint)(*vecClosestTo, vecClosestRestartPoint, fRotation);
}

/**
 * Find the closest hospital restart point to a specific point
 * @param vecClosestTo CVector * containing the position you want to find the closest restart point to
 * @param vecClosestRestartPoint CVector * that returns the position of the closest restart point
 * @param fRotation FLOAT * that returns the rotation at which the restart point restarts the player
 */
VOID CRestartSA::FindClosestHospitalRestartPoint(CVector* vecClosestTo, CVector* vecClosestRestartPoint, FLOAT* fRotation)
{
    DEBUG_TRACE("VOID CRestartSA::FindClosestHospitalRestartPoint ( CVector * vecClosestTo, CVector * vecClosestRestartPoint, FLOAT * fRotation )");

    // CRestart::FindClosestHospitalRestartPoint
    ((void(__cdecl*)(CVector, CVector*, float*))FUNC_FindClosestHospitalRestartPoint)(*vecClosestTo, vecClosestRestartPoint, fRotation);
}

/**
 * Add a police restart point so that the player restarts here (if its their nearest restart point) when they've
 * been "busted".
 * @param vecPosition CVector * containing the desired position for the restart point
 * @param fRotation FLOAT containing the desired initial rotation for the player
 */
VOID CRestartSA::AddPoliceRestartPoint(CVector* vecPosition, FLOAT fRotation)
{
    DEBUG_TRACE("VOID CRestartSA::AddPoliceRestartPoint ( CVector * vecPosition, FLOAT fRotation )");

    // CRestart::AddPoliceRestartPoint
    ((void(__cdecl*)(CVector&, float, unsigned int))FUNC_AddPoliceRestartPoint)(*vecPosition, fRotation, 0);
}

/**
 * Add a hospital restart point so that the player restarts here (if its their nearest restart point) when they've
 * been "wasted".
 * @param vecPosition CVector * containing the desired position for the restart point
 * @param fRotation FLOAT containing the desired initial rotation for the player
 */
VOID CRestartSA::AddHospitalRestartPoint(CVector* vecPosition, FLOAT fRotation)
{
    DEBUG_TRACE("VOID CRestartSA::AddHospitalRestartPoint ( CVector * vecPosition, FLOAT fRotation )");

    // CRestart::AddHospitalRestartPoint
    ((void(__cdecl*)(CVector&, float, unsigned int))FUNC_AddHospitalRestartPoint)(*vecPosition, fRotation, 0);
}

/**
 * Checks if the player is in the process of respawning after being arrested
 * @return BOOL TRUE if they have just been arested and not yet respawned, FALSE otherwise.
 */
BOOL CRestartSA::IsRestartingAfterArrest()
{
    DEBUG_TRACE("BOOL CRestartSA::IsRestartingAfterArrest  (  )");

    // CRestart::AddHospitalRestartPoint
    return ((bool(__cdecl*)())FUNC_IsRestartingAfterArrest)();
}

/**
 * Checks if the player is in the process of respawning after dying
 * @return BOOL TRUE if they have just died and not yet respawned, FALSE otherwise.
 */
BOOL CRestartSA::IsRestartingAfterDeath()
{
    DEBUG_TRACE("BOOL CRestartSA::IsRestartingAfterDeath (  )");

    // CRestart::IsRestartingAfterDeath
    return ((bool(__cdecl*)())FUNC_IsRestartingAfterDeath)();
}
