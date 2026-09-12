/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CWeaponStat.cpp
 *  PURPOSE:     Header file for custom weapon stats.
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CWeaponStat.h"
#include "CWeaponStatManager.h"
#include "CGame.h"

uint CWeaponStat::ms_uiAllWeaponStatsRevision = 0;

CWeaponStat::CWeaponStat(eWeaponType weaponType, eWeaponSkill skillLevel)
{
    // LoadDefault only fills weapon types 22 to 42, so start blank so the
    // other types read fixed zeros instead of uninitialized memory.
    memset(&tWeaponStats, 0, sizeof(tWeaponStats));
    this->weaponType = weaponType;
    this->skillLevel = skillLevel;
    this->m_bChanged = false;
}
CWeaponStat::CWeaponStat()
{
    memset(&tWeaponStats, 0, sizeof(tWeaponStats));
    this->weaponType = WEAPONTYPE_UNARMED;
    this->skillLevel = WEAPONSKILL_STD;
    this->m_bChanged = false;
}
CWeaponStat::~CWeaponStat()
{
}

eWeaponType CWeaponStat::GetWeaponType()
{
    return this->weaponType;
}

eWeaponSkill CWeaponStat::GetWeaponSkillLevel()
{
    return this->skillLevel;
}

void CWeaponStat::SetWeaponType(eWeaponType weaponType)
{
    ms_uiAllWeaponStatsRevision++;
    this->weaponType = weaponType;
}

void CWeaponStat::SetWeaponSkillLevel(eWeaponSkill skillLevel)
{
    ms_uiAllWeaponStatsRevision++;
    this->skillLevel = skillLevel;
}

void CWeaponStat::ToggleFlagBits(DWORD flagBits)
{
    HandleFlagsValueChange(tWeaponStats.m_nFlags ^ flagBits);
}

void CWeaponStat::SetFlagBits(DWORD flagBits)
{
    HandleFlagsValueChange(tWeaponStats.m_nFlags | flagBits);
}

void CWeaponStat::ClearFlagBits(DWORD flagBits)
{
    HandleFlagsValueChange(tWeaponStats.m_nFlags & ~flagBits);
}

void CWeaponStat::HandleFlagsValueChange(DWORD newValue)
{
    // Fixup anim group if dual wield flag is changing
    if (IsFlagSet(WEAPONTYPE_TWIN_PISTOLS) && !(newValue & WEAPONTYPE_TWIN_PISTOLS))
    {
        // Clearing dual wield flag

        // if it can support this anim group
        if ((GetWeaponType() >= WEAPONTYPE_PISTOL && GetWeaponType() <= WEAPONTYPE_SNIPERRIFLE) || GetWeaponType() == WEAPONTYPE_MINIGUN)
        {
            // Revert anim group to default
            CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats(GetWeaponType(), GetWeaponSkillLevel());
            if (pOriginalWeaponInfo)
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

    // The anim flag checks below need the original data for this type;
    // types without it keep the anim bits off, so scripts cannot add
    // anims the base weapon does not support.

    // Don't allow setting of anim reload flag unless original has it
    if (!IsFlagSet(WEAPONTYPE_ANIM_RELOAD) && (newValue & WEAPONTYPE_ANIM_RELOAD))
    {
        CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats(GetWeaponType(), GetWeaponSkillLevel());
        if (!pOriginalWeaponInfo || !pOriginalWeaponInfo->IsFlagSet(WEAPONTYPE_ANIM_RELOAD))
            newValue &= ~WEAPONTYPE_ANIM_RELOAD;
    }

    // Don't allow setting anim crouch flag unless original has it
    if (!IsFlagSet(WEAPONTYPE_ANIM_CROUCHFIRE) && (newValue & WEAPONTYPE_ANIM_CROUCHFIRE))
    {
        CWeaponStat* pOriginalWeaponInfo = g_pGame->GetWeaponStatManager()->GetOriginalWeaponStats(GetWeaponType(), GetWeaponSkillLevel());
        if (!pOriginalWeaponInfo || !pOriginalWeaponInfo->IsFlagSet(WEAPONTYPE_ANIM_CROUCHFIRE))
            newValue &= ~WEAPONTYPE_ANIM_CROUCHFIRE;
    }

    ms_uiAllWeaponStatsRevision++;
    tWeaponStats.m_nFlags = newValue;
}
