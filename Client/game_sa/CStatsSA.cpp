/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStatsSA.cpp
 *  PURPOSE:     Game statistics
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CStatsSA.h"

float CStatsSA::GetStatValue(unsigned short usIndex)
{
    DWORD dwFunc = FUNC_GetStatValue;
    float fReturn = 0.0f;
    DWORD dwStatIndex = usIndex;

    // clang-format off
    fReturn = gta_call_address<decltype(fReturn)>(dwFunc, dwStatIndex);
    // clang-format on
    return fReturn;
}

void CStatsSA::ModifyStat(unsigned short usIndex, float fAmmount)
{
    DWORD dwFunc = FUNC_ModifyStat;
    DWORD dwStatIndex = usIndex;

    // clang-format off
    gta_call_address(dwFunc, dwStatIndex, fAmmount);
    // clang-format on
}

void CStatsSA::SetStatValue(unsigned short usIndex, float fAmmount)
{
    DWORD dwFunc = FUNC_SetStatValue;
    DWORD dwStatIndex = usIndex;

    // clang-format off
    gta_call_address(dwFunc, dwStatIndex, fAmmount);
    // clang-format on
}

unsigned short CStatsSA::GetSkillStatIndex(eWeaponType type)
{
    int   weaponType = (int)type;
    int   iIndex;
    DWORD dwFunc = FUNC_CWeaponInfo_GetSkillStatIndex;
    // clang-format off
    iIndex = gta_call_address<decltype(iIndex)>(dwFunc, weaponType);
    // clang-format on
    return (unsigned short)iIndex;
}
