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

uint CWeaponStat::ms_uiAllWeaponStatsRevision = 0;

CWeaponStat::CWeaponStat ( eWeaponType weaponType, eWeaponSkill skillLevel )
{
    this->weaponType = weaponType;
    this->skillLevel = skillLevel;
    this->m_bChanged = false;
}
CWeaponStat::CWeaponStat ( )
{
    this->m_bChanged = false;
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
    ms_uiAllWeaponStatsRevision++;
    this->weaponType = weaponType;
}

void CWeaponStat::SetWeaponSkillLevel ( eWeaponSkill skillLevel )
{
    ms_uiAllWeaponStatsRevision++;
    this->skillLevel = skillLevel;
}

void CWeaponStat::ToggleFlagBits ( DWORD flagBits )
{
    HandleFlagsValueChange( tWeaponStats.m_nFlags ^ flagBits );
}

void CWeaponStat::SetFlagBits ( DWORD flagBits )
{
    HandleFlagsValueChange( tWeaponStats.m_nFlags | flagBits );
}

void CWeaponStat::ClearFlagBits ( DWORD flagBits )
{
    HandleFlagsValueChange( tWeaponStats.m_nFlags & ~flagBits );
}

void CWeaponStat::HandleFlagsValueChange ( DWORD newValue )
{
    // Fixup anim group if dual wield flag is changing
    if ( IsFlagSet( WEAPONTYPE_TWIN_PISTOLS ) && !( newValue & WEAPONTYPE_TWIN_PISTOLS ) )
    {
        // Clearing dual wield flag

        // if it can support this anim group
        if ( ( GetWeaponType() >= WEAPONTYPE_PISTOL && GetWeaponType() <= WEAPONTYPE_SNIPERRIFLE ) || GetWeaponType() == WEAPONTYPE_MINIGUN )
        {
            // Revert anim group to default
            CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( GetWeaponType(), GetWeaponSkillLevel() );
            SetAnimGroup ( pOriginalWeaponInfo->GetAnimGroup ( ) );
        }
    }
    else
    if ( !IsFlagSet( WEAPONTYPE_TWIN_PISTOLS ) && ( newValue & WEAPONTYPE_TWIN_PISTOLS ) )
    {
        // Setting dual wield flag

        // if it can support this anim group
        if ( ( GetWeaponType() >= WEAPONTYPE_PISTOL && GetWeaponType() <= WEAPONTYPE_SNIPERRIFLE ) || GetWeaponType() == WEAPONTYPE_MINIGUN )
        {
            // sawn off shotgun anim group
            SetAnimGroup ( 17 );
        }
    }

    // Don't allow setting of anim reload flag unless original has it
    if ( !IsFlagSet( WEAPONTYPE_ANIM_RELOAD ) && ( newValue & WEAPONTYPE_ANIM_RELOAD ) )
    {
        CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( GetWeaponType(), GetWeaponSkillLevel() );
        if ( !pOriginalWeaponInfo->IsFlagSet( WEAPONTYPE_ANIM_RELOAD ) )
            newValue &= ~WEAPONTYPE_ANIM_RELOAD;
    }

    // Don't allow setting anim crouch flag unless original has it
    if ( !IsFlagSet( WEAPONTYPE_ANIM_CROUCHFIRE ) && ( newValue & WEAPONTYPE_ANIM_CROUCHFIRE ) )
    {
        CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats( GetWeaponType(), GetWeaponSkillLevel() );
        if ( !pOriginalWeaponInfo->IsFlagSet( WEAPONTYPE_ANIM_CROUCHFIRE ) )
            newValue &= ~WEAPONTYPE_ANIM_CROUCHFIRE;
    }

    ms_uiAllWeaponStatsRevision++; 
    tWeaponStats.m_nFlags = newValue;
}
