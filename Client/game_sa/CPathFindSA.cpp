/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPathFindSA.cpp
 *  PURPOSE:     Game entity path finding logic
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <CVector.h>
#include "CPathFindSA.h"

void CPathFindSA::SwitchRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable)
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    DWORD dwEnable = !bEnable;

    DWORD dwFunc = FUNC_SwitchRoadsOffInArea;

    _asm
    {
        mov     ecx, CLASS_CPathFind
        push    0
        push    1
        push    dwEnable
        push    fZ2
        push    fZ1
        push    fY2
        push    fY1
        push    fX2
        push    fX1
        call    dwFunc
    }
}

void CPathFindSA::SwitchPedRoadsOffInArea(CVector* vecAreaCorner1, CVector* vecAreaCorner2, bool bEnable)
{
    float fX1 = vecAreaCorner1->fX;
    float fY1 = vecAreaCorner1->fY;
    float fZ1 = vecAreaCorner1->fZ;

    float fX2 = vecAreaCorner2->fX;
    float fY2 = vecAreaCorner2->fY;
    float fZ2 = vecAreaCorner2->fZ;

    DWORD dwEnable = !bEnable;

    DWORD dwFunc = FUNC_SwitchPedRoadsOffInArea;

    _asm
    {
        mov     ecx, CLASS_CPathFind
        push    0
        push    dwEnable
        push    fZ2
        push    fZ1
        push    fY2
        push    fY1
        push    fX2
        push    fX1
        call    dwFunc
    }
}

void CPathFindSA::SetPedDensity(float fPedDensity)
{
    MemPutFast<float>(VAR_PedDensityMultiplier, fPedDensity);
}

void CPathFindSA::SetVehicleDensity(float fVehicleDensity)
{
    MemPutFast<float>(VAR_CarDensityMultiplier, fVehicleDensity);
}
