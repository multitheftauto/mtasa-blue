/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CStatsSA.h
 *  PURPOSE:     Header file for game statistics class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CStats.h>

#define FUNC_GetStatValue                   0x558E40
#define FUNC_ModifyStat                     0x55D090
#define FUNC_SetStatValue                   0x55A070
#define FUNC_CWeaponInfo_GetSkillStatIndex  0x743CD0

class CStatsSA : public CStats
{
public:
    float GetStatValue(unsigned short usIndex);
    void  ModifyStat(unsigned short usIndex, float fAmmount);
    void  SetStatValue(unsigned short usIndex, float fAmmount);

    unsigned short GetSkillStatIndex(eWeaponType type);
};
