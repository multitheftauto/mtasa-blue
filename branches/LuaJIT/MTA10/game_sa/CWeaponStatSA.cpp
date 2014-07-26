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

void CWeaponStatSA::ToggleFlagBits ( DWORD flagBits )
{
    HandleFlagsValueChange( pWeaponStats->m_nFlags ^ flagBits );
}

void CWeaponStatSA::SetFlagBits ( DWORD flagBits )
{
    HandleFlagsValueChange( pWeaponStats->m_nFlags | flagBits );
}

void CWeaponStatSA::ClearFlagBits ( DWORD flagBits )
{
    HandleFlagsValueChange( pWeaponStats->m_nFlags & ~flagBits );
}

void CWeaponStatSA::HandleFlagsValueChange ( DWORD newValue )
{
    // Fixup anim group if dual wield flag is changing
    if ( IsFlagSet( WEAPONTYPE_TWIN_PISTOLS ) && !( newValue & WEAPONTYPE_TWIN_PISTOLS ) )
    {
        // Clearing dual wield flag

        // if it can support this anim group
        if ( ( GetWeaponType() >= WEAPONTYPE_PISTOL && GetWeaponType() <= WEAPONTYPE_SNIPERRIFLE ) || GetWeaponType() == WEAPONTYPE_MINIGUN )
        {
            // Revert anim group to default
            CWeaponStat* pOriginalWeaponInfo = pGame->GetWeaponStatManager()->GetOriginalWeaponStats( GetWeaponType(), GetWeaponSkillLevel() );
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
        CWeaponStat* pOriginalWeaponInfo = pGame->GetWeaponStatManager()->GetOriginalWeaponStats( GetWeaponType(), GetWeaponSkillLevel() );
        if ( !pOriginalWeaponInfo->IsFlagSet( WEAPONTYPE_ANIM_RELOAD ) )
            newValue &= ~WEAPONTYPE_ANIM_RELOAD;
    }

    // Don't allow setting anim crouch flag unless original has it
    if ( !IsFlagSet( WEAPONTYPE_ANIM_CROUCHFIRE ) && ( newValue & WEAPONTYPE_ANIM_CROUCHFIRE ) )
    {
        CWeaponStat* pOriginalWeaponInfo = pGame->GetWeaponStatManager()->GetOriginalWeaponStats( GetWeaponType(), GetWeaponSkillLevel() );
        if ( !pOriginalWeaponInfo->IsFlagSet( WEAPONTYPE_ANIM_CROUCHFIRE ) )
            newValue &= ~WEAPONTYPE_ANIM_CROUCHFIRE;
    }

    pWeaponStats->m_nFlags = newValue;
}
