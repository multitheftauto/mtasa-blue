/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CStatsSA.cpp
 *  PURPOSE:     Game statistics
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CStatsSA.h"

float CStatsSA::GetStatValue(ushort usIndex)
{
    DWORD dwFunc = 0x558E40;
    float fReturn = 0.0f;
    DWORD dwStatIndex = usIndex;
    _asm
    {
        push    dwStatIndex
        call    dwFunc
        add     esp, 4
        fstp    fReturn
    }
    return fReturn;
}

void CStatsSA::ModifyStat(ushort usIndex, float fAmmount)
{
    DWORD dwFunc = 0x55D090;
    DWORD dwStatIndex = usIndex;
    _asm
    {
        push    fAmmount
        push    dwStatIndex
        call    dwFunc
        add     esp, 8
    }
}

void CStatsSA::SetStatValue(ushort usIndex, float fAmmount)
{
    DWORD dwFunc = 0x55A070;
    DWORD dwStatIndex = usIndex;
    _asm
    {
        push    fAmmount
        push    dwStatIndex
        call    dwFunc
        add     esp, 8
    }
}

ushort CStatsSA::GetSkillStatIndex(eWeaponType type)
{
    int   weaponType = (int)type;
    int   iIndex = 0;
    DWORD dwFunc = 0x743CD0;
    _asm
    {
        push    weaponType
        call    dwFunc
        add     esp, 0x4
        mov     iIndex, eax
    }
    return (ushort)iIndex;
}
