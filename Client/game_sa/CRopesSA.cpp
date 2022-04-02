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
#define MAX_NUM_ROPES 8
DWORD dwDurationAddress = 0x558D1El;
CRopesSAInterface (&CRopesSA::ms_aRopes)[MAX_NUM_ROPES] = *(CRopesSAInterface(*)[MAX_NUM_ROPES])0xB768B8;

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
    // Set it back for SA in case we ever do some other implementation.
    MemPut((DWORD*)(dwDurationAddress), 4000);
    return iReturn;
}

void CRopesSA::RemoveEntityRope(CEntitySAInterface* pEntity)
{
    CRopesSAInterface* pRope = nullptr;

    for (uint i = 0; i <= ROPES_COUNT; i++)
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

int32 CRopesSA::FindRope(CEntitySAInterface* id)
{
    for (auto ropeId = 0; ropeId < MAX_NUM_ROPES; ropeId++)
    {
        if (ms_aRopes[ropeId].m_ucRopeType != eRopeType::NONE && ms_aRopes[ropeId].m_pRopeEntity == id)
            return ropeId;
    }
    return -1;
}

CRopesSAInterface& CRopesSA::GetRope(int32 index)
{
    //assert(index >= 0);
    if (index >= 0)
        return ms_aRopes[index];
}
