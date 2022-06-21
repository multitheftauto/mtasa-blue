/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStatsSA.cpp
 *  PURPOSE:     Game statistics
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

float CStatsSA::GetStatValue(unsigned short usIndex)
{
    // CStats::GetStatValue
    return ((float(__cdecl*)(unsigned short))FUNC_GetStatValue)(usIndex);
}

void CStatsSA::ModifyStat(unsigned short usIndex, float fAmmount)
{
    // CStats::ModifyStat
    ((void(__cdecl*)(unsigned short, float))FUNC_ModifyStat)(usIndex, fAmmount);
}

void CStatsSA::IncrementStat(unsigned short usIndex, float fAmmount)
{
    // CStats::IncrementStat
    ((void(__cdecl*)(unsigned short, float))FUNC_IncrementStat)(usIndex, fAmmount);
}

void CStatsSA::DecrementStat(unsigned short usIndex, float fAmmount)
{
    // CStats::DecrementStat
    ((void(__cdecl*)(unsigned short, float))FUNC_DecrementStat)(usIndex, fAmmount);
}

void CStatsSA::SetStatValue(unsigned short usIndex, float fAmmount)
{
    // CStats::SetStatValue
    ((void(__cdecl*)(unsigned short, float))FUNC_SetStatValue)(usIndex, fAmmount);
}

unsigned short CStatsSA::GetSkillStatIndex(eWeaponType type)
{
    // CStats::SetStatValue
    return ((int(__cdecl*)(eWeaponType))FUNC_CWeaponInfo_GetSkillStatIndex)(type);
}
