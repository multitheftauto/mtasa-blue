/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponStat.cpp
*  PURPOSE:     Header file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#include "StdInc.h"

CWeaponStat::CWeaponStat ( eWeaponType weaponType, eWeaponSkill skillLevel )
{
    this->weaponType = weaponType;
    this->skillLevel = skillLevel;
}
CWeaponStat::~CWeaponStat ( )
{

}

eWeaponType CWeaponStat::GetWeaponType ()
{
    return this->weaponType;
}

eWeaponSkill CWeaponStat::GetWeaponSkillLevel ()
{
    return this->skillLevel;
}

void CWeaponStat::SetWeaponType ( eWeaponType weaponType )
{
    this->weaponType = weaponType;
}

void CWeaponStat::SetWeaponSkillLevel ( eWeaponSkill skillLevel )
{
    this->skillLevel = skillLevel;
}