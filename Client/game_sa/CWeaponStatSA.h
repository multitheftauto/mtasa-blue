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
    void SetFlags(int iFlags) { pWeaponStats->m_nFlags = iFlags; }
    bool IsFlagSet(DWORD flag) { return ((pWeaponStats->m_nFlags & flag) > 0 ? true : false); }
    int  GetFlags() { return pWeaponStats->m_nFlags; }

    eWeaponModel GetModel() { return (eWeaponModel)pWeaponStats->m_modelId; }
    void         SetModel(int iModel) { pWeaponStats->m_modelId = (int)iModel; }

    eWeaponModel GetModel2() { return (eWeaponModel)pWeaponStats->m_modelId2; }
    void         SetModel2(int iModel) { pWeaponStats->m_modelId2 = (int)iModel; }

    CWeaponInfoSAInterface* GetInterface() { return pWeaponStats; };

    float GetWeaponRange() { return pWeaponStats->m_fWeaponRange; };
    void  SetWeaponRange(float fRange) { pWeaponStats->m_fWeaponRange = fRange; };

    float GetTargetRange() { return pWeaponStats->m_fTargetRange; };
    void  SetTargetRange(float fRange) { pWeaponStats->m_fTargetRange = fRange; };

    CVector* GetFireOffset() { return &pWeaponStats->m_vecFireOffset; };
    void     SetFireOffset(CVector* vecFireOffset) { pWeaponStats->m_vecFireOffset = *vecFireOffset; };

    short GetDamagePerHit() { return pWeaponStats->m_nDamage; };
    void  SetDamagePerHit(short sDamagePerHit) { pWeaponStats->m_nDamage = sDamagePerHit; };

    float GetAccuracy() { return pWeaponStats->m_fAccuracy; };
    void  SetAccuracy(float fAccuracy) { pWeaponStats->m_fAccuracy = fAccuracy; };

    short GetMaximumClipAmmo() { return pWeaponStats->m_nAmmo; };
    void  SetMaximumClipAmmo(short sAccuracy) { pWeaponStats->m_nAmmo = sAccuracy; };

    float GetMoveSpeed() { return pWeaponStats->m_fMoveSpeed; };
    void  SetMoveSpeed(float fMoveSpeed) { pWeaponStats->m_fMoveSpeed = fMoveSpeed; };

    // projectile/areaeffect only
    float GetFiringSpeed() { return pWeaponStats->m_fSpeed; };
    void  SetFiringSpeed(float fFiringSpeed) { pWeaponStats->m_fSpeed = fFiringSpeed; };

    // area effect only
    float GetRadius() { return pWeaponStats->m_fRadius; };
    void  SetRadius(float fRadius) { pWeaponStats->m_fRadius = fRadius; };

    float GetLifeSpan() { return pWeaponStats->m_fLifeSpan; };
    void  SetLifeSpan(float fLifeSpan) { pWeaponStats->m_fLifeSpan = fLifeSpan; };

    float GetSpread() { return pWeaponStats->m_fSpread; };
    void  SetSpread(float fSpread) { pWeaponStats->m_fSpread = fSpread; };

    float GetAnimBreakoutTime() { return pWeaponStats->m_animBreakoutTime; };
    void  SetAnimBreakoutTime(float fBreakoutTime) { pWeaponStats->m_animBreakoutTime = fBreakoutTime; };

    eWeaponSlot GetSlot() { return (eWeaponSlot)pWeaponStats->m_nWeaponSlot; };
    void        SetSlot(eWeaponSlot dwSlot) { pWeaponStats->m_nWeaponSlot = (eWeaponSlot)dwSlot; };

    eWeaponSkill GetSkill() { return pWeaponStats->m_SkillLevel; }
    void         SetSkill(eWeaponSkill weaponSkill) { pWeaponStats->m_SkillLevel = weaponSkill; }

    float GetRequiredStatLevel() { return static_cast<float>(pWeaponStats->m_nReqStatLevel); }
    void  SetRequiredStatLevel(float fStatLevel) { pWeaponStats->m_nReqStatLevel = static_cast<int>(fStatLevel); }
    void  SetRequiredStatLevel(int iStatLevel) { pWeaponStats->m_nReqStatLevel = iStatLevel; }

    DWORD GetAnimGroup() { return pWeaponStats->m_animGroup; }
    void  SetAnimGroup(DWORD dwAnimGroup) { pWeaponStats->m_animGroup = dwAnimGroup; }

    eFireType GetFireType() { return pWeaponStats->m_eFireType; }
    void      SetFireType(eFireType type) { pWeaponStats->m_eFireType = type; }

    // Floats
    float GetWeaponAnimLoopStart() { return pWeaponStats->m_animLoopStart; }
    void  SetWeaponAnimLoopStart(float animLoopStart) { pWeaponStats->m_animLoopStart = animLoopStart; }

    float GetWeaponAnimLoopStop() { return pWeaponStats->m_animLoopEnd; }
    void  SetWeaponAnimLoopStop(float animLoopEnd) { pWeaponStats->m_animLoopEnd = animLoopEnd; }

    float GetWeaponAnimLoopFireTime() { return pWeaponStats->m_animFireTime; }
    void  SetWeaponAnimLoopFireTime(float animFireTime) { pWeaponStats->m_animFireTime = animFireTime; }

    float GetWeaponAnim2LoopStart() { return pWeaponStats->m_anim2LoopStart; }
    void  SetWeaponAnim2LoopStart(float anim2LoopStart) { pWeaponStats->m_anim2LoopStart = anim2LoopStart; }

    float GetWeaponAnim2LoopStop() { return pWeaponStats->m_anim2LoopEnd; }
    void  SetWeaponAnim2LoopStop(float anim2LoopEnd) { pWeaponStats->m_anim2LoopEnd = anim2LoopEnd; }

    float GetWeaponAnim2LoopFireTime() { return pWeaponStats->m_anim2FireTime; }
    void  SetWeaponAnim2LoopFireTime(float anim2FireTime) { pWeaponStats->m_anim2FireTime = anim2FireTime; }

    float GetWeaponAnimBreakoutTime() { return pWeaponStats->m_animBreakoutTime; }
    void  SetWeaponAnimBreakoutTime(float animBreakoutTime) { pWeaponStats->m_animBreakoutTime = animBreakoutTime; }

    float GetWeaponSpeed() { return pWeaponStats->m_fSpeed; }
    void  SetWeaponSpeed(float fSpeed) { pWeaponStats->m_fSpeed = fSpeed; }

    float GetWeaponRadius() { return pWeaponStats->m_fRadius; }
    void  SetWeaponRadius(float fRadius) { pWeaponStats->m_fRadius = fRadius; }

    // Ints
    short GetAimOffsetIndex() { return pWeaponStats->m_nAimOffsetIndex; }
    void  SetAimOffsetIndex(short sIndex) { pWeaponStats->m_nAimOffsetIndex = sIndex; }

    BYTE GetDefaultCombo() { return pWeaponStats->m_defaultCombo; }
    void SetDefaultCombo(BYTE defaultCombo) { pWeaponStats->m_defaultCombo = defaultCombo; }

    BYTE GetCombosAvailable() { return pWeaponStats->m_nCombosAvailable; }
    void SetCombosAvailable(BYTE nCombosAvailable) { pWeaponStats->m_nCombosAvailable = nCombosAvailable; }

private:
    void HandleFlagsValueChange(DWORD newValue);

    eWeaponType             weaponType;
    eWeaponSkill            skillLevel;
    CWeaponInfoSAInterface* pWeaponStats;
};
