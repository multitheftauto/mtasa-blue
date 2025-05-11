/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeaponStatManager.h
 *  PURPOSE:     Weapon Stat Manager Interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CWeaponStat.h"

class CWeaponStatManager
{
public:
    virtual CWeaponStat* GetWeaponStats(eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD) = 0;
    virtual CWeaponStat* GetWeaponStatsFromSkillLevel(eWeaponType type, float fSkillLevel) = 0;
    virtual CWeaponStat* GetOriginalWeaponStats(eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD) = 0;
    virtual void         Init() = 0;
    virtual void         ResetLists() = 0;
    virtual bool         LoadDefault(CWeaponStat* pDest, eWeaponType weaponType, eWeaponSkill skill = WEAPONSKILL_STD) = 0;
    virtual void         CreateWeaponStat(CWeaponInfo* pInterface, eWeaponType weaponType, eWeaponSkill weaponSkill) = 0;
    virtual CWeaponStat* CreateWeaponStatUnlisted(eWeaponType weaponType, eWeaponSkill weaponSkill) = 0;
    virtual eWeaponSkill GetWeaponSkillFromSkillLevel(eWeaponType type, float fSkillLevel) = 0;
};
