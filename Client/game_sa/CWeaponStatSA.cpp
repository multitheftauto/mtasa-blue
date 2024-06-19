/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponStatSA.cpp
 *  PURPOSE:     Header file for custom weapon stats.
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CGameSA.h"
#include "CWeaponStatManagerSA.h"
#include "CWeaponStatSA.h"

extern CGameSA* pGame;

// For Data Storage and won't apply changes.
CWeaponStatSA::CWeaponStatSA(eWeaponType weaponType, eWeaponSkill skillLevel) : m_weaponType{weaponType}, m_skillLevel{skillLevel}
{
    m_pWeaponStats = new CWeaponInfoSAInterface();
}

// For scripting API and applies changes.
CWeaponStatSA::CWeaponStatSA(CWeaponInfoSA* pWeaponInfo, eWeaponType weaponType, eWeaponSkill skillLevel) : m_weaponType{weaponType}, m_skillLevel{skillLevel}
{
    m_pWeaponStats = pWeaponInfo->GetInterface();
}

CWeaponStatSA::~CWeaponStatSA()
{
}

eWeaponType CWeaponStatSA::GetWeaponType()
{
    return m_weaponType;
}

eWeaponSkill CWeaponStatSA::GetWeaponSkillLevel()
{
    return m_skillLevel;
}

void CWeaponStatSA::SetWeaponType(eWeaponType weaponType)
{
    m_weaponType = weaponType;
}

void CWeaponStatSA::SetWeaponSkillLevel(eWeaponSkill skillLevel)
{
    m_skillLevel = skillLevel;
}

void CWeaponStatSA::ToggleFlagBits(DWORD flagBits)
{
    HandleFlagsValueChange(m_pWeaponStats->m_nFlags ^ flagBits);
}

void CWeaponStatSA::SetFlagBits(DWORD flagBits)
{
    HandleFlagsValueChange(m_pWeaponStats->m_nFlags | flagBits);
}

void CWeaponStatSA::ClearFlagBits(DWORD flagBits)
{
    HandleFlagsValueChange(m_pWeaponStats->m_nFlags & ~flagBits);
}

void CWeaponStatSA::HandleFlagsValueChange(DWORD newValue)
{
    // Fixup anim group if dual wield flag is changing
    if (IsFlagSet(WEAPONTYPE_TWIN_PISTOLS) && !(newValue & WEAPONTYPE_TWIN_PISTOLS))
    {
        // Clearing dual wield flag

        // if it can support this anim group
        if ((GetWeaponType() >= WEAPONTYPE_PISTOL && GetWeaponType() <= WEAPONTYPE_SNIPERRIFLE) || GetWeaponType() == WEAPONTYPE_MINIGUN)
        {
            // Revert anim group to default
            CWeaponStat* pOriginalWeaponInfo = pGame->GetWeaponStatManager()->GetOriginalWeaponStats(GetWeaponType(), GetWeaponSkillLevel());
            SetAnimGroup(pOriginalWeaponInfo->GetAnimGroup());
        }
    }
    else if (!IsFlagSet(WEAPONTYPE_TWIN_PISTOLS) && (newValue & WEAPONTYPE_TWIN_PISTOLS))
    {
        // Setting dual wield flag

        // if it can support this anim group
        if ((GetWeaponType() >= WEAPONTYPE_PISTOL && GetWeaponType() <= WEAPONTYPE_SNIPERRIFLE) || GetWeaponType() == WEAPONTYPE_MINIGUN)
        {
            // sawn off shotgun anim group
            SetAnimGroup(17);
        }
    }

    // Don't allow setting of anim reload flag unless original has it
    if (!IsFlagSet(WEAPONTYPE_ANIM_RELOAD) && (newValue & WEAPONTYPE_ANIM_RELOAD))
    {
        CWeaponStat* pOriginalWeaponInfo = pGame->GetWeaponStatManager()->GetOriginalWeaponStats(GetWeaponType(), GetWeaponSkillLevel());
        if (!pOriginalWeaponInfo->IsFlagSet(WEAPONTYPE_ANIM_RELOAD))
            newValue &= ~WEAPONTYPE_ANIM_RELOAD;
    }

    // Don't allow setting anim crouch flag unless original has it
    if (!IsFlagSet(WEAPONTYPE_ANIM_CROUCHFIRE) && (newValue & WEAPONTYPE_ANIM_CROUCHFIRE))
    {
        CWeaponStat* pOriginalWeaponInfo = pGame->GetWeaponStatManager()->GetOriginalWeaponStats(GetWeaponType(), GetWeaponSkillLevel());
        if (!pOriginalWeaponInfo->IsFlagSet(WEAPONTYPE_ANIM_CROUCHFIRE))
            newValue &= ~WEAPONTYPE_ANIM_CROUCHFIRE;
    }

    m_pWeaponStats->m_nFlags = newValue;
}
