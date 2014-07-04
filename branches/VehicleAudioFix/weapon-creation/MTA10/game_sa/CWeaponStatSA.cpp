/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponStatSA.cpp
*  PURPOSE:     Header file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#include "StdInc.h"

CWeaponStatSA::CWeaponStatSA ( eWeaponType weaponType, eWeaponSkill skillLevel )
{
    // For Data Storage and won't apply changes.
    this->pWeaponStats = new CWeaponInfoSAInterface();
    this->weaponType = weaponType;
    this->skillLevel = skillLevel;
}
CWeaponStatSA::CWeaponStatSA ( CWeaponInfoSA * pWeaponInfo, eWeaponType weaponType, eWeaponSkill skillLevel )
{
    // For scripting API and applies changes..
    this->pWeaponStats = pWeaponInfo->GetInterface();
    this->weaponType = weaponType;
    this->skillLevel = skillLevel;
}

CWeaponStatSA::~CWeaponStatSA ( )
{

}

eWeaponType CWeaponStatSA::GetWeaponType ()
{
    return this->weaponType;
}

eWeaponSkill CWeaponStatSA::GetWeaponSkillLevel ()
{
    return this->skillLevel;
}

void CWeaponStatSA::SetWeaponType ( eWeaponType weaponType )
{
    this->weaponType = weaponType;
}

void CWeaponStatSA::SetWeaponSkillLevel ( eWeaponSkill skillLevel )
{
    this->skillLevel = skillLevel;
}