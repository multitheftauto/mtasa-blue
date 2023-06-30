/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/game_sa/CStatsSA.h
 *  PURPOSE:     Header file for game statistics class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CStats.h>

class CStatsSA : public CStats
{
public:
    float GetStatValue(ushort usIndex);
    void  ModifyStat(ushort usIndex, float fAmmount);
    void  SetStatValue(ushort usIndex, float fAmmount);

    ushort GetSkillStatIndex(eWeaponType type);
};
