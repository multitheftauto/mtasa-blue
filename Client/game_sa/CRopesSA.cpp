/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRopesSA.cpp
 *  PURPOSE:     Rope entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRopesSA.h"

DWORD dwDurationAddress = 0x558D1El;

CRopesSAInterface (&CRopesSA::ms_aRopes)[8] = *(CRopesSAInterface(*)[8])0xB768B8;

int CRopesSA::CreateRopeForSwatPed(const CVector& vecPosition, DWORD dwDuration)
{
    int      iReturn;
    DWORD    dwFunc = FUNC_CRopes_CreateRopeForSwatPed;
    CVector* pvecPosition = const_cast<CVector*>(&vecPosition);
    // First Push @ 0x558D1D is the duration.
    MemPut((void*)(dwDurationAddress), dwDuration);
    _asm
    {
        push    pvecPosition
        call    dwFunc
        add     esp, 0x4
        mov     iReturn, eax
    }
    //   Set it back for SA in case we ever do some other implementation.
    MemPut((DWORD*)(dwDurationAddress), 4000);
    return iReturn;
}

void CRopesSA::RemoveEntityRope(CEntitySAInterface* pEntity)
{
    CRopesSAInterface* pRope = nullptr;

    for (uint i = 0; i < ROPES_COUNT; i++)
    {
        if (ms_aRopes[i].m_pRopeEntity == pEntity)
        {
            pRope = &ms_aRopes[i];
            break;
        }
    }

    if (pRope)
    {
        auto CRope_Remove = (void(__thiscall*)(CRopesSAInterface*))0x556780;
        CRope_Remove(pRope);
    }
}
