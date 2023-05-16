/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponStatSA.h
 *  PURPOSE:     Source file for custom weapon stats.
 *
 *****************************************************************************/

#pragma once
#include <game/CWeaponStat.h>
#include "CWeaponInfoSA.h"

class CWeaponStatSA : public CWeaponStat
{
public:
    CWeaponStatSA(){};
    CWeaponStatSA(eWeaponType weaponType, eWeaponSkill skillLevel);
    CWeaponStatSA(CWeaponInfoSA* pWeaponInfo, eWeaponType weaponType, eWeaponSkill skillLevel);
    ~CWeaponStatSA();

    eWeaponType  GetWeaponType();
    eWeaponSkill GetWeaponSkillLevel();

    void SetWeaponType(eWeaponType weaponType);
    void SetWeaponSkillLevel(eWeaponSkill skillLevel);

    //
    // Interface Sets and Gets
    //

    void ToggleFlagBits(DWORD flagBits);
    void SetFlagBits(DWORD flagBits);
    void ClearFlagBits(DWORD flagBits);
    // For initialization only
    void SetFlags(int iFlags) { m_pWeaponStats->m_nFlags = iFlags; }
    bool IsFlagSet(DWORD flag) { return ((m_pWeaponStats->m_nFlags & flag) > 0 ? true : false); }
    int  GetFlags() { return m_pWeaponStats->m_nFlags; }

    eWeaponModel GetModel() { return (eWeaponModel)m_pWeaponStats->m_modelId; }
    void         SetModel(int iModel) { m_pWeaponStats->m_modelId = (int)iModel; }

    eWeaponModel GetModel2() { return (eWeaponModel)m_pWeaponStats->m_modelId2; }
    void         SetModel2(int iModel) { m_pWeaponStats->m_modelId2 = (int)iModel; }

    CWeaponInfoSAInterface* GetInterface() { return m_pWeaponStats; };

    float GetWeaponRange() { return m_pWeaponStats->m_fWeaponRange; };
    void  SetWeaponRange(float fRange) { m_pWeaponStats->m_fWeaponRange = fRange; };

    float GetTargetRange() { return m_pWeaponStats->m_fTargetRange; };
    void  SetTargetRange(float fRange) { m_pWeaponStats->m_fTargetRange = fRange; };

    CVector* GetFireOffset() { return &m_pWeaponStats->m_vecFireOffset; };
    void     SetFireOffset(CVector* vecFireOffset) { m_pWeaponStats->m_vecFireOffset = *vecFireOffset; };

    short GetDamagePerHit() { return m_pWeaponStats->m_nDamage; };
    void  SetDamagePerHit(short sDamagePerHit) { m_pWeaponStats->m_nDamage = sDamagePerHit; };

    float GetAccuracy() { return m_pWeaponStats->m_fAccuracy; };
    void  SetAccuracy(float fAccuracy) { m_pWeaponStats->m_fAccuracy = fAccuracy; };

    short GetMaximumClipAmmo() { return m_pWeaponStats->m_nAmmo; };
    void  SetMaximumClipAmmo(short sAccuracy) { m_pWeaponStats->m_nAmmo = sAccuracy; };

    float GetMoveSpeed() { return m_pWeaponStats->m_fMoveSpeed; };
    void  SetMoveSpeed(float fMoveSpeed) { m_pWeaponStats->m_fMoveSpeed = fMoveSpeed; };

    // projectile/areaeffect only
    float GetFiringSpeed() { return m_pWeaponStats->m_fSpeed; };
    void  SetFiringSpeed(float fFiringSpeed) { m_pWeaponStats->m_fSpeed = fFiringSpeed; };

    // area effect only
    float GetRadius() { return m_pWeaponStats->m_fRadius; };
    void  SetRadius(float fRadius) { m_pWeaponStats->m_fRadius = fRadius; };

    float GetLifeSpan() { return m_pWeaponStats->m_fLifeSpan; };
    void  SetLifeSpan(float fLifeSpan) { m_pWeaponStats->m_fLifeSpan = fLifeSpan; };

    float GetSpread() { return m_pWeaponStats->m_fSpread; };
    void  SetSpread(float fSpread) { m_pWeaponStats->m_fSpread = fSpread; };

    float GetAnimBreakoutTime() { return m_pWeaponStats->m_animBreakoutTime; };
    void  SetAnimBreakoutTime(float fBreakoutTime) { m_pWeaponStats->m_animBreakoutTime = fBreakoutTime; };

    eWeaponSlot GetSlot() { return (eWeaponSlot)m_pWeaponStats->m_nWeaponSlot; };
    void        SetSlot(eWeaponSlot dwSlot) { m_pWeaponStats->m_nWeaponSlot = (eWeaponSlot)dwSlot; };

    eWeaponSkill GetSkill() { return m_pWeaponStats->m_SkillLevel; }
    void         SetSkill(eWeaponSkill weaponSkill) { m_pWeaponStats->m_SkillLevel = weaponSkill; }

    float GetRequiredStatLevel() { return static_cast<float>(m_pWeaponStats->m_nReqStatLevel); }
    void  SetRequiredStatLevel(float fStatLevel) { m_pWeaponStats->m_nReqStatLevel = static_cast<int>(fStatLevel); }
    void  SetRequiredStatLevel(int iStatLevel) { m_pWeaponStats->m_nReqStatLevel = iStatLevel; }

    DWORD GetAnimGroup() { return m_pWeaponStats->m_animGroup; }
    void  SetAnimGroup(DWORD dwAnimGroup) { m_pWeaponStats->m_animGroup = dwAnimGroup; }

    eFireType GetFireType() { return m_pWeaponStats->m_eFireType; }
    void      SetFireType(eFireType type) { m_pWeaponStats->m_eFireType = type; }

    // Floats
    float GetWeaponAnimLoopStart() { return m_pWeaponStats->m_animLoopStart; }
    void  SetWeaponAnimLoopStart(float animLoopStart) { m_pWeaponStats->m_animLoopStart = animLoopStart; }

    float GetWeaponAnimLoopStop() { return m_pWeaponStats->m_animLoopEnd; }
    void  SetWeaponAnimLoopStop(float animLoopEnd) { m_pWeaponStats->m_animLoopEnd = animLoopEnd; }

    float GetWeaponAnimLoopFireTime() { return m_pWeaponStats->m_animFireTime; }
    void  SetWeaponAnimLoopFireTime(float animFireTime) { m_pWeaponStats->m_animFireTime = animFireTime; }

    float GetWeaponAnim2LoopStart() { return m_pWeaponStats->m_anim2LoopStart; }
    void  SetWeaponAnim2LoopStart(float anim2LoopStart) { m_pWeaponStats->m_anim2LoopStart = anim2LoopStart; }

    float GetWeaponAnim2LoopStop() { return m_pWeaponStats->m_anim2LoopEnd; }
    void  SetWeaponAnim2LoopStop(float anim2LoopEnd) { m_pWeaponStats->m_anim2LoopEnd = anim2LoopEnd; }

    float GetWeaponAnim2LoopFireTime() { return m_pWeaponStats->m_anim2FireTime; }
    void  SetWeaponAnim2LoopFireTime(float anim2FireTime) { m_pWeaponStats->m_anim2FireTime = anim2FireTime; }

    float GetWeaponAnimBreakoutTime() { return m_pWeaponStats->m_animBreakoutTime; }
    void  SetWeaponAnimBreakoutTime(float animBreakoutTime) { m_pWeaponStats->m_animBreakoutTime = animBreakoutTime; }

    float GetWeaponSpeed() { return m_pWeaponStats->m_fSpeed; }
    void  SetWeaponSpeed(float fSpeed) { m_pWeaponStats->m_fSpeed = fSpeed; }

    float GetWeaponRadius() { return m_pWeaponStats->m_fRadius; }
    void  SetWeaponRadius(float fRadius) { m_pWeaponStats->m_fRadius = fRadius; }

    // Ints
    short GetAimOffsetIndex() { return m_pWeaponStats->m_nAimOffsetIndex; }
    void  SetAimOffsetIndex(short sIndex) { m_pWeaponStats->m_nAimOffsetIndex = sIndex; }

    BYTE GetDefaultCombo() { return m_pWeaponStats->m_defaultCombo; }
    void SetDefaultCombo(BYTE defaultCombo) { m_pWeaponStats->m_defaultCombo = defaultCombo; }

    BYTE GetCombosAvailable() { return m_pWeaponStats->m_nCombosAvailable; }
    void SetCombosAvailable(BYTE nCombosAvailable) { m_pWeaponStats->m_nCombosAvailable = nCombosAvailable; }

private:
    void HandleFlagsValueChange(DWORD newValue);

    eWeaponType             m_weaponType;
    eWeaponSkill            m_skillLevel;
    CWeaponInfoSAInterface* m_pWeaponStats;
};
