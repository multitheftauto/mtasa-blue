/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CWeaponInfoSA.h
 *  PURPOSE:     Header file for weapon type information class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CWeaponInfo.h>
#include <CVector.h>

class CEntitySAInterface;

// Flame shot array for flamethrower flames and maybe molotovs
#define ARRAY_CFlameShotInfo                0xC89690
#define MAX_FLAME_SHOT_INFOS                100

class CFlameShotInfo
{
public:
    eWeaponType         weaponType;
    CVector             vecPosition;
    CVector             vecSize;
    float               fRadius;
    CEntitySAInterface* pInstigator;
    float               fLifeSpan;
    uint8               ucFlag1;
    uint8               ucFlag2;
    uint16              usPad;
};
static_assert(sizeof(CFlameShotInfo) == 0x2c, "Invalid size for CFlameShotInfo");

class CWeaponInfoSAInterface            // 112 byte long class
{
public:
    eFireType m_eFireType;            // type - instant hit (e.g. pistol), projectile (e.g. rocket launcher), area effect (e.g. flame thrower)

    float m_fTargetRange;            // max targeting range
    float m_fWeaponRange;            // absolute gun range / default melee attack range
    int   m_modelId;                 // modelinfo id
    int   m_modelId2;                // second modelinfo id

    eWeaponSlot m_nWeaponSlot;
    int         m_nFlags;            // flags defining characteristics

    // instead of storing pointers directly to anims, use anim association groups
    // NOTE: this is used for stealth kill anims for melee weapons
    DWORD m_animGroup;

    //////////////////////////////////
    // Gun Data
    /////////////////////////////////
    short   m_nAmmo;                    // ammo in one clip
    short   m_nDamage;                  // damage inflicted per hit
    CVector m_vecFireOffset;            // offset from weapon origin to projectile starting point

    // skill settings
    eWeaponSkill m_SkillLevel;               // what's the skill level of this weapontype
    int          m_nReqStatLevel;            // what stat level is required for this skill level
    float        m_fAccuracy;                // modify accuracy of weapon
    float        m_fMoveSpeed;               // how fast can move with weapon

    // anim timings
    float m_animLoopStart;            // start of animation loop
    float m_animLoopEnd;              // end of animation loop
    float m_animFireTime;             // time in animation when weapon should be fired

    float m_anim2LoopStart;            // start of animation2 loop
    float m_anim2LoopEnd;              // end of animation2 loop
    float m_anim2FireTime;             // time in animation2 when weapon should be fired

    float m_animBreakoutTime;            // time after which player can break out of attack and run off

    // projectile/area effect specific info
    float m_fSpeed;               // speed of projectile
    float m_fRadius;              // radius affected
    float m_fLifeSpan;            // time taken for shot to dissipate
    float m_fSpread;              // angle inside which shots are created

    short m_nAimOffsetIndex;            // index into array of aiming offsets
    //////////////////////////////////
    // Melee Data
    /////////////////////////////////
    BYTE m_defaultCombo;                // base combo for this melee weapon
    BYTE m_nCombosAvailable;            // how many further combos are available
};

class CWeaponInfoSA : public CWeaponInfo
{
private:
    eWeaponType             m_weaponType;
    CWeaponInfoSAInterface* m_pInterface;

public:
    CWeaponInfoSA(CWeaponInfoSAInterface* pInterface, eWeaponType weaponType) : m_pInterface{pInterface}, m_weaponType{weaponType} {};

    // stolen from R*'s CPedIK :)
    // DO NOT USE.
    void SetFlags(int iFlags) { m_pInterface->m_nFlags = iFlags; }
    // END
    void  SetFlag(DWORD flag) { m_pInterface->m_nFlags |= flag; }
    void  ClearFlag(DWORD flag) { m_pInterface->m_nFlags &= ~flag; }
    bool  IsFlagSet(DWORD flag) { return ((m_pInterface->m_nFlags & flag) > 0 ? true : false); }
    short GetFlags() { return m_pInterface->m_nFlags; }

    eWeaponModel GetModel() { return (eWeaponModel)m_pInterface->m_modelId; }
    eWeaponModel GetModel2() { return (eWeaponModel)m_pInterface->m_modelId2; }

    CWeaponInfoSAInterface* GetInterface() { return m_pInterface; };

    float GetWeaponRange() { return m_pInterface->m_fWeaponRange; };
    void  SetWeaponRange(float fRange) { m_pInterface->m_fWeaponRange = fRange; };

    float GetTargetRange() { return m_pInterface->m_fTargetRange; };
    void  SetTargetRange(float fRange) { m_pInterface->m_fTargetRange = fRange; };

    CVector* GetFireOffset() { return &m_pInterface->m_vecFireOffset; };
    void     SetFireOffset(CVector* vecFireOffset) { MemCpyFast(&m_pInterface->m_vecFireOffset, vecFireOffset, sizeof(CVector)); };

    short GetDamagePerHit() { return m_pInterface->m_nDamage; };
    void  SetDamagePerHit(short sDamagePerHit) { m_pInterface->m_nDamage = sDamagePerHit; };

    float GetAccuracy() { return m_pInterface->m_fAccuracy; };
    void  SetAccuracy(float fAccuracy) { m_pInterface->m_fAccuracy = fAccuracy; };

    short GetMaximumClipAmmo() { return m_pInterface->m_nAmmo; };
    void  SetMaximumClipAmmo(short sAmmo) { m_pInterface->m_nAmmo = sAmmo; };

    float GetMoveSpeed() { return m_pInterface->m_fMoveSpeed; };
    void  SetMoveSpeed(float fMoveSpeed) { m_pInterface->m_fMoveSpeed = fMoveSpeed; };

    // projectile/areaeffect only
    float GetFiringSpeed() { return m_pInterface->m_fSpeed; };
    void  SetFiringSpeed(float fFiringSpeed) { m_pInterface->m_fSpeed = fFiringSpeed; };

    // area effect only
    float GetRadius() { return m_pInterface->m_fRadius; };
    void  SetRadius(float fRadius) { m_pInterface->m_fRadius = fRadius; };

    float GetLifeSpan() { return m_pInterface->m_fLifeSpan; };
    void  SetLifeSpan(float fLifeSpan) { m_pInterface->m_fLifeSpan = fLifeSpan; };

    float GetSpread() { return m_pInterface->m_fSpread; };
    void  SetSpread(float fSpread) { m_pInterface->m_fSpread = fSpread; };

    float GetAnimBreakoutTime() { return m_pInterface->m_animBreakoutTime; };
    void  SetAnimBreakoutTime(float fBreakoutTime) { m_pInterface->m_animBreakoutTime = fBreakoutTime; };

    eWeaponSlot GetSlot() { return (eWeaponSlot)m_pInterface->m_nWeaponSlot; };
    void        SetSlot(eWeaponSlot dwSlot) { m_pInterface->m_nWeaponSlot = (eWeaponSlot)dwSlot; };

    eWeaponSkill GetSkill() { return m_pInterface->m_SkillLevel; }
    void         SetSkill(eWeaponSkill weaponSkill) { m_pInterface->m_SkillLevel = weaponSkill; }

    float GetRequiredStatLevel() { return static_cast<float>(m_pInterface->m_nReqStatLevel); }
    void  SetRequiredStatLevel(float fStatLevel) { m_pInterface->m_nReqStatLevel = static_cast<int>(fStatLevel); }

    DWORD GetAnimGroup() { return m_pInterface->m_animGroup; }
    void  SetAnimGroup(DWORD dwAnimGroup) { m_pInterface->m_animGroup = dwAnimGroup; }

    eFireType GetFireType() { return m_pInterface->m_eFireType; }

    // Floats
    float GetWeaponAnimLoopStart() { return m_pInterface->m_animLoopStart; }
    float GetWeaponAnimLoopStop() { return m_pInterface->m_animLoopEnd; }
    float GetWeaponAnimLoopFireTime() { return m_pInterface->m_animFireTime; }
    void  SetWeaponAnimLoopFireTime(float animFireTime) { m_pInterface->m_animFireTime = animFireTime; }

    float GetWeaponAnim2LoopStart() { return m_pInterface->m_anim2LoopStart; }
    float GetWeaponAnim2LoopStop() { return m_pInterface->m_anim2LoopEnd; }
    float GetWeaponAnim2LoopFireTime() { return m_pInterface->m_anim2FireTime; }

    float GetWeaponAnimBreakoutTime() { return m_pInterface->m_animBreakoutTime; }

    float GetWeaponRadius() { return m_pInterface->m_fRadius; }

    // Ints
    short GetAimOffsetIndex() { return m_pInterface->m_nAimOffsetIndex; }

    BYTE GetDefaultCombo() { return m_pInterface->m_defaultCombo; }
    BYTE GetCombosAvailable() { return m_pInterface->m_nCombosAvailable; }
};
