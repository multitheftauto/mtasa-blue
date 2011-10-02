/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponStatManager.h
*  PURPOSE:     Header file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CWEAPONSTATMANAGER_H
#define __CWEAPONSTATMANAGER_H

#include <list>
#include <logic/CCommon.h>
// Straight out of CGameSA.h
#define     CLASSSIZE_WeaponInfo            112         // ##SA##
#define     NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define     NUM_WeaponInfosOtherSkill       11
#define     NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)

#define WEAPON_STAT_MAX 112

class CWeaponStatManager
{
public:
                                CWeaponStatManager              ( void );
                                ~CWeaponStatManager             ( void );
    CWeaponStat*                GetWeaponStats                  ( eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD );
    CWeaponStat*                GetOriginalWeaponStats          ( eWeaponType type, eWeaponSkill skill = WEAPONSKILL_STD );


private:

    std::list<CWeaponStat*>                                     m_OriginalWeaponData;
    std::list<CWeaponStat*>                                     m_WeaponData;

};

#endif