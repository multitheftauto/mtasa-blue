/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_HookDestructors.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    // Ensure both ends of a line are valid
    void LimitLinePoints( CVector& vecStart, CVector& vecEnd )
    {
        bool bStartValid = IsValidPosition( vecStart );
        bool bEndValid = IsValidPosition( vecEnd );
        if ( bStartValid && bEndValid )
            return;
        if ( !bStartValid && !bEndValid )
        {
            vecStart = CVector( 0, 0, 100 );
            vecEnd = CVector( 0, 0, 100.01f );
        }
        else
        if ( !bStartValid )
        {
            vecStart = vecEnd + CVector( 0, 0, 0.01f );
        }
        else
        {
            vecEnd = vecStart + CVector( 0, 0, 0.01f );
        }
    }
}

////////////////////////////////////////////////////////////////
//
// OnCWorld_ProcessLineOfSight
//
// Ensure position args in range
//
////////////////////////////////////////////////////////////////
void _cdecl OnCWorld_ProcessLineOfSight ( CVector* pvecStart, CVector* pvecEnd )
{
    LimitLinePoints( *pvecStart, *pvecEnd );
}


// Hook info
#define HOOKPOS_CWorld_ProcessLineOfSight        0x56BA00
#define HOOKSIZE_CWorld_ProcessLineOfSight       12
DWORD RETURN_CWorld_ProcessLineOfSight =         0x56BA0C;
void _declspec(naked) HOOK_CWorld_ProcessLineOfSight()
{
    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnCWorld_ProcessLineOfSight
        add     esp, 4*2
        popad

        sub     esp, 60h 
        cmp     word ptr ds:[0B7CD78h], 0FFFFh 
        jmp     RETURN_CWorld_ProcessLineOfSight
    }
}


////////////////////////////////////////////////////////////////
//
// CWorld_GetIsLineOfSightClear
//
// Ensure position args in range
//
////////////////////////////////////////////////////////////////
void _cdecl OnCWorld_GetIsLineOfSightClear ( CVector* pvecStart, CVector* pvecEnd )
{
    LimitLinePoints( *pvecStart, *pvecEnd );
}


// Hook info
#define HOOKPOS_CWorld_GetIsLineOfSightClear        0x56A490
#define HOOKSIZE_CWorld_GetIsLineOfSightClear       12
DWORD RETURN_CWorld_GetIsLineOfSightClear =         0x56A49C;
void _declspec(naked) HOOK_CWorld_GetIsLineOfSightClear()
{
    _asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnCWorld_GetIsLineOfSightClear
        add     esp, 4*2
        popad

        sub     esp, 5Ch
        cmp     word ptr ds:[0B7CD78h], 0FFFFh
        jmp     RETURN_CWorld_GetIsLineOfSightClear
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_FixLineOfSightArgs ( void )
{
   EZHookInstall ( CWorld_ProcessLineOfSight );
   EZHookInstall ( CWorld_GetIsLineOfSightClear );
}
