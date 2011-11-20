/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CWeaponStatManager.h
*  PURPOSE:     Weapon Stat Manager Interface
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWEAPONSTATMANAGER_H
#define __CWEAPONSTATMANAGER_H
#include "CWeaponStat.h"

class CWeaponStatManager
{
public:
    virtual CWeaponStat*                GetWeaponStats                  ( eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD ) = 0;
    virtual CWeaponStat*                GetWeaponStatsFromSkillLevel     ( eWeaponType type, float fSkillLevel ) = 0;
    virtual CWeaponStat*                GetOriginalWeaponStats          ( eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD ) = 0;
    virtual void                        Init                            ( void ) = 0;
    virtual void                        ResetLists                      ( void ) = 0;
    virtual bool                        LoadDefault                     ( CWeaponStat* pDest, eWeaponType weaponType, eWeaponSkill skill = WEAPONSKILL_STD  ) = 0;
    virtual void                        CreateWeaponStat                ( CWeaponInfo* pInterface, eWeaponType weaponType, eWeaponSkill weaponSkill ) = 0;

};

#endif
