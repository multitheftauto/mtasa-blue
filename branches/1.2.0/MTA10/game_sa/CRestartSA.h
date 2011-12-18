/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRestartSA.h
*  PURPOSE:     Header file for restart points class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_RESTART
#define __CGAMESA_RESTART

#include <game/CRestart.h>
#include <CVector.h>
#include "Common.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FUNC_CancelOverrideRestart                  0x460800    // not used
#define FUNC_OverrideNextRestart                    0x4607d0    // not used
#define FUNC_FindClosestPoliceRestartPoint          0x460a50
#define FUNC_FindClosestHospitalRestartPoint        0x460850
#define FUNC_AddPoliceRestartPoint                  0x460780
#define FUNC_AddHospitalRestartPoint                0x460730
// belong in CPlayerInfo
#define FUNC_IsRestartingAfterArrest                0x56e560
#define FUNC_IsRestartingAfterDeath                 0x56e550
/**
 * \todo 10: Update for SA
 */
#define VAR_OverrideNextRestart                     0xA10AF8
#define VAR_OverrideNextRestartPosition             0x975344
#define VAR_OverrideNextRestartRotation             0x978E54

class CRestartSA : public CRestart
{
    VOID            CancelOverrideRestart (  );
    VOID            OverrideNextRestart ( CVector * vecPosition, FLOAT fRotation );
    VOID            FindClosestPoliceRestartPoint ( CVector * vecClosestTo, CVector * vecClosestRestartPoint, FLOAT * fRotation );
    VOID            FindClosestHospitalRestartPoint ( CVector * vecClosestTo, CVector * vecClosestRestartPoint, FLOAT * fRotation );
    VOID            AddPoliceRestartPoint ( CVector * vecPosition, FLOAT fRotation );
    VOID            AddHospitalRestartPoint ( CVector * vecPosition, FLOAT fRotation );
    // these really belong in CPlayerInfo, but make more sense here
    BOOL            IsRestartingAfterArrest  (  );
    BOOL            IsRestartingAfterDeath (  );
};

#endif
