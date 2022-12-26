/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAutomobileSA.cpp
 *  PURPOSE:     Automobile vehicle entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CAutomobileSA.h"

/**
 * \todo Spawn automobiles with engine off
 */
CAutomobileSA::CAutomobileSA(eVehicleTypes dwModelID, unsigned char ucVariation, unsigned char ucVariation2) : CVehicleSA(dwModelID, ucVariation, ucVariation2)
{
    // create our mirror classes
    for (int i = 0; i < MAX_DOORS; i++)
        this->door[i] = new CDoorSA(&((CAutomobileSAInterface*)this->GetInterface())->m_doors[i]);

}

CAutomobileSA::CAutomobileSA(CAutomobileSAInterface* automobile)
{
    // just so it can be passed on to CVehicle
}

CAutomobileSA::~CAutomobileSA()
{
    for (int i = 0; i < MAX_DOORS; i++)
    {
        if (this->door[i])
            delete this->door[i];
    }
}

bool CAutomobileSA::BurstTyre(DWORD dwTyreID)
{
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_BurstTyre;
    bool  bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    dwTyreID
        call    dwFunc;
        mov     bReturn, al
    }

    return bReturn;
}

bool CAutomobileSA::BreakTowLink()
{
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_BreakTowLink;
    bool  bReturn;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

bool CAutomobileSA::IsComponentPresent(int iComponentID)
{
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CAutomobile_IsComponentPresent;
    bool  bReturn;

    _asm
    {
        mov     ecx, dwThis
        push    iComponentID
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

CPhysical* CAutomobileSA::SpawnFlyingComponent(int iCarNodeIndex, int iUnknown)
{
    DWORD      dwThis = (DWORD)GetInterface();
    DWORD      dwFunc = FUNC_CAutomobile_SpawnFlyingComponent;
    CPhysical* pReturn;

    _asm
    {
        mov     ecx, dwThis
        push    iUnknown
        push    iCarNodeIndex
        call    dwFunc
        mov     pReturn, eax
    }

    return pReturn;
}

CDoor* CAutomobileSA::GetDoor(eDoors doorID)
{
    return this->door[doorID];
}
