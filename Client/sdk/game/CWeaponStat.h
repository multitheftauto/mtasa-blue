/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CWeaponStat.h
 *  PURPOSE:     Weapon Stat Manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CWeaponInfo.h"

class CVector;

class CWeaponStat
{
public:
    virtual eWeaponType  GetWeaponType() = 0;
    virtual eWeaponSkill GetWeaponSkillLevel() = 0;

    virtual void SetWeaponType(eWeaponType weaponType) = 0;
    virtual void SetWeaponSkillLevel(eWeaponSkill skillLevel) = 0;

    //
    // Interface Sets and Gets
    //

    virtual void ToggleFlagBits(DWORD flagBits) = 0;
    virtual void SetFlagBits(DWORD flagBits) = 0;
    virtual void ClearFlagBits(DWORD flagBits) = 0;
    // For initialization only
    virtual void SetFlags(int iFlags) = 0;
    virtual bool IsFlagSet(DWORD flag) = 0;
    virtual int  GetFlags() = 0;

    virtual eWeaponModel GetModel() = 0;
    virtual void         SetModel(int iModel) = 0;

    virtual eWeaponModel GetModel2() = 0;
    virtual void         SetModel2(int iModel) = 0;

    virtual float GetWeaponRange() = 0;
    virtual void  SetWeaponRange(float fRange) = 0;

    virtual float GetTargetRange() = 0;
    virtual void  SetTargetRange(float fRange) = 0;

    virtual CVector* GetFireOffset() = 0;
    virtual void     SetFireOffset(CVector* vecFireOffset) = 0;

    virtual short GetDamagePerHit() = 0;
    virtual void  SetDamagePerHit(short sDamagePerHit) = 0;

    virtual float GetAccuracy() = 0;
    virtual void  SetAccuracy(float fAccuracy) = 0;

    virtual short GetMaximumClipAmmo() = 0;
    virtual void  SetMaximumClipAmmo(short sAccuracy) = 0;

    virtual float GetMoveSpeed() = 0;
    virtual void  SetMoveSpeed(float fMoveSpeed) = 0;

    // projectile/areaeffect only
    virtual float GetFiringSpeed() = 0;
    virtual void  SetFiringSpeed(float fFiringSpeed) = 0;

    // area effect only
    virtual float GetRadius() = 0;
    virtual void  SetRadius(float fRadius) = 0;

    virtual float GetLifeSpan() = 0;
    virtual void  SetLifeSpan(float fLifeSpan) = 0;

    virtual float GetSpread() = 0;
    virtual void  SetSpread(float fSpread) = 0;

    virtual float GetAnimBreakoutTime() = 0;
    virtual void  SetAnimBreakoutTime(float fBreakoutTime) = 0;

    virtual eWeaponSlot GetSlot() = 0;
    virtual void        SetSlot(eWeaponSlot dwSlot) = 0;

    virtual eWeaponSkill GetSkill() = 0;
    virtual void         SetSkill(eWeaponSkill weaponSkill) = 0;

    virtual float GetRequiredStatLevel() = 0;
    virtual void  SetRequiredStatLevel(float fStatLevel) = 0;
    virtual void  SetRequiredStatLevel(int iStatLevel) = 0;

    virtual DWORD GetAnimGroup() = 0;
    virtual void  SetAnimGroup(DWORD dwAnimGroup) = 0;

    virtual eFireType GetFireType() = 0;
    virtual void      SetFireType(eFireType type) = 0;

    // Floats
    virtual float GetWeaponAnimLoopStart() = 0;
    virtual void  SetWeaponAnimLoopStart(float animLoopStart) = 0;

    virtual float GetWeaponAnimLoopStop() = 0;
    virtual void  SetWeaponAnimLoopStop(float animLoopEnd) = 0;

    virtual float GetWeaponAnimLoopFireTime() = 0;
    virtual void  SetWeaponAnimLoopFireTime(float animFireTime) = 0;

    virtual float GetWeaponAnim2LoopStart() = 0;
    virtual void  SetWeaponAnim2LoopStart(float anim2LoopStart) = 0;

    virtual float GetWeaponAnim2LoopStop() = 0;
    virtual void  SetWeaponAnim2LoopStop(float anim2LoopEnd) = 0;

    virtual float GetWeaponAnim2LoopFireTime() = 0;
    virtual void  SetWeaponAnim2LoopFireTime(float anim2FireTime) = 0;

    virtual float GetWeaponAnimBreakoutTime() = 0;
    virtual void  SetWeaponAnimBreakoutTime(float animBreakoutTime) = 0;

    virtual float GetWeaponSpeed() = 0;
    virtual void  SetWeaponSpeed(float fSpeed) = 0;

    virtual float GetWeaponRadius() = 0;
    virtual void  SetWeaponRadius(float fRadius) = 0;

    // Ints
    virtual short GetAimOffsetIndex() = 0;
    virtual void  SetAimOffsetIndex(short sIndex) = 0;

    virtual BYTE GetDefaultCombo() = 0;
    virtual void SetDefaultCombo(BYTE defaultCombo) = 0;

    virtual BYTE GetCombosAvailable() = 0;
    virtual void SetCombosAvailable(BYTE nCombosAvailable) = 0;
};
