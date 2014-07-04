/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRopesSA.cpp
*  PURPOSE:     Rope entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
DWORD dwDurationAddress = 0x558D1El;

int CRopesSA::CreateRopeForSwatPed ( const CVector & vecPosition, DWORD dwDuration )
{
    int iReturn;
    DWORD dwFunc = FUNC_CRopes_CreateRopeForSwatPed;
    CVector * pvecPosition = const_cast < CVector * > ( &vecPosition );
    // First Push @ 0x558D1D is the duration.
    MemPut((void*)(dwDurationAddress), dwDuration);
    _asm
    {
        push    pvecPosition
        call    dwFunc
        add     esp, 0x4
        mov     iReturn, eax
    }
    // Set it back for SA in case we ever do some other implementation.
    MemPut((DWORD*)(dwDurationAddress), 4000);
    return iReturn;
}