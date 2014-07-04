/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponInfoSA.h
*  PURPOSE:     Header file for weapon type information class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_WEAPONINFO
#define __CGAMESA_WEAPONINFO

#include <game/CWeaponInfo.h>
#include <CVector.h>
#include "Common.h"

// Flame shot array for flamethrower flames and maybe molotovs
#define ARRAY_CFlameShotInfo                0xC89690
#define MAX_FLAME_SHOT_INFOS                64

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

class CWeaponInfoSAInterface // 112 byte long class
{
public:
    eFireType   m_eFireType;        // type - instant hit (e.g. pistol), projectile (e.g. rocket launcher), area effect (e.g. flame thrower)

    FLOAT       m_fTargetRange;     // max targeting range
    FLOAT       m_fWeaponRange;     // absolute gun range / default melee attack range
    int         m_modelId;          // modelinfo id
    int         m_modelId2;         // second modelinfo id
    
    eWeaponSlot m_nWeaponSlot;
    int         m_nFlags;           // flags defining characteristics

    // instead of storing pointers directly to anims, use anim association groups
    // NOTE: this is used for stealth kill anims for melee weapons
    DWORD       m_animGroup;

    //////////////////////////////////
    // Gun Data
    /////////////////////////////////
    short       m_nAmmo;                // ammo in one clip
    short       m_nDamage;              // damage inflicted per hit
    CVector m_vecFireOffset;        // offset from weapon origin to projectile starting point
    
    // skill settings
    eWeaponSkill m_SkillLevel;          // what's the skill level of this weapontype
    int         m_nReqStatLevel;        // what stat level is required for this skill level
    FLOAT       m_fAccuracy;            // modify accuracy of weapon
    FLOAT       m_fMoveSpeed;           // how fast can move with weapon

    // anim timings
    FLOAT       m_animLoopStart;        // start of animation loop
    FLOAT       m_animLoopEnd;          // end of animation loop
    FLOAT       m_animFireTime;         // time in animation when weapon should be fired

    FLOAT       m_anim2LoopStart;       // start of animation2 loop
    FLOAT       m_anim2LoopEnd;         // end of animation2 loop
    FLOAT       m_anim2FireTime;        // time in animation2 when weapon should be fired

    FLOAT       m_animBreakoutTime;     // time after which player can break out of attack and run off
    
    // projectile/area effect specific info
    FLOAT       m_fSpeed;               // speed of projectile
    FLOAT       m_fRadius;              // radius affected
    FLOAT       m_fLifeSpan;            // time taken for shot to dissipate
    FLOAT       m_fSpread;              // angle inside which shots are created
    
    short       m_nAimOffsetIndex;      // index into array of aiming offsets
    //////////////////////////////////
    // Melee Data
    /////////////////////////////////
    BYTE        m_defaultCombo;         // base combo for this melee weapon
    BYTE        m_nCombosAvailable;     // how many further combos are available
};

class CWeaponInfoSA : public CWeaponInfo
{
private:
    eWeaponType                 weaponType;
    CWeaponInfoSAInterface      * internalInterface;
public:
    // constructor
    CWeaponInfoSA                                           ( CWeaponInfoSAInterface * weaponInfoInterface, eWeaponType weaponType )
    {
        internalInterface = weaponInfoInterface;  
        weaponType = weaponType; 
    };

    // stolen from R*'s CPedIK :)
    // DO NOT USE.
    void                        SetFlags                    ( int iFlags )                  { internalInterface->m_nFlags = iFlags; }
    // END
    void                        SetFlag                     ( DWORD flag )                  { internalInterface->m_nFlags |= flag; }
    void                        ClearFlag                   ( DWORD flag )                  { internalInterface->m_nFlags &= ~flag; }
    bool                        IsFlagSet                   ( DWORD flag )                  { return ((internalInterface->m_nFlags & flag) > 0 ? true : false); }
    short                       GetFlags                    ( void )                        { return internalInterface->m_nFlags; }

    eWeaponModel                GetModel()                                                  { return (eWeaponModel)internalInterface->m_modelId; }
    eWeaponModel                GetModel2()                                                 { return (eWeaponModel)internalInterface->m_modelId2; }

    CWeaponInfoSAInterface *    GetInterface                ( void )                        { return internalInterface; };

    float                       GetWeaponRange              ( void )                        { return internalInterface->m_fWeaponRange; };
    void                        SetWeaponRange              ( float fRange )                { internalInterface->m_fWeaponRange = fRange; };

    float                       GetTargetRange              ( void )                        { return internalInterface->m_fTargetRange; };
    void                        SetTargetRange              ( float fRange )                { internalInterface->m_fTargetRange = fRange; };

    CVector *                   GetFireOffset               ( void )                        { return &internalInterface->m_vecFireOffset; };
    void                        SetFireOffset               ( CVector * vecFireOffset )     { MemCpyFast (&internalInterface->m_vecFireOffset, vecFireOffset, sizeof(CVector)); };

    short                       GetDamagePerHit             ( void )                        { return internalInterface->m_nDamage; };
    void                        SetDamagePerHit             ( short sDamagePerHit )         { internalInterface->m_nDamage = sDamagePerHit; };

    float                       GetAccuracy                 ( void )                        { return internalInterface->m_fAccuracy; };
    void                        SetAccuracy                 ( float fAccuracy )             { internalInterface->m_fAccuracy = fAccuracy; };

    short                       GetMaximumClipAmmo          ( void )                        { return internalInterface->m_nAmmo; };
    void                        SetMaximumClipAmmo          ( short sAmmo )                 { internalInterface->m_nAmmo = sAmmo; };

    float                       GetMoveSpeed                ( void )                        { return internalInterface->m_fMoveSpeed; };
    void                        SetMoveSpeed                ( float fMoveSpeed )            { internalInterface->m_fMoveSpeed = fMoveSpeed; };

    // projectile/areaeffect only
    float                       GetFiringSpeed              ( void )                        { return internalInterface->m_fSpeed; };
    void                        SetFiringSpeed              ( float fFiringSpeed )          { internalInterface->m_fSpeed = fFiringSpeed; };

    // area effect only
    float                       GetRadius                   ( void )                        { return internalInterface->m_fRadius; };
    void                        SetRadius                   ( float fRadius )               { internalInterface->m_fRadius = fRadius; };

    float                       GetLifeSpan                 ( void )                        { return internalInterface->m_fLifeSpan; };
    void                        SetLifeSpan                 ( float fLifeSpan )             { internalInterface->m_fLifeSpan = fLifeSpan; };

    float                       GetSpread                   ( void )                        { return internalInterface->m_fSpread; };
    void                        SetSpread                   ( float fSpread )               { internalInterface->m_fSpread = fSpread; };

    float                       GetAnimBreakoutTime         ( void )                        { return internalInterface->m_animBreakoutTime; };
    void                        SetAnimBreakoutTime         ( float fBreakoutTime )         { internalInterface->m_animBreakoutTime = fBreakoutTime; };

    eWeaponSlot                 GetSlot                     ( void )                        { return (eWeaponSlot)internalInterface->m_nWeaponSlot; };
    void                        SetSlot                     ( eWeaponSlot dwSlot )          { internalInterface->m_nWeaponSlot = (eWeaponSlot)dwSlot; };

    eWeaponSkill                GetSkill                    ( void )                        { return internalInterface->m_SkillLevel; }
    void                        SetSkill                    ( eWeaponSkill weaponSkill )    { internalInterface->m_SkillLevel = weaponSkill; }

    float                       GetRequiredStatLevel        ( void )                        { return static_cast < float > ( internalInterface->m_nReqStatLevel ); }
    void                        SetRequiredStatLevel        ( float fStatLevel )            { internalInterface->m_nReqStatLevel = static_cast < int > ( fStatLevel ); }

    DWORD                       GetAnimGroup                ( void )                        { return internalInterface->m_animGroup; }
    void                        SetAnimGroup                ( DWORD dwAnimGroup )           { internalInterface->m_animGroup = dwAnimGroup; }

    eFireType                   GetFireType                 ( void )                        { return internalInterface->m_eFireType; }

    // Floats
    float                       GetWeaponAnimLoopStart      ( void )                        { return internalInterface->m_animLoopStart; }
    float                       GetWeaponAnimLoopStop       ( void )                        { return internalInterface->m_animLoopEnd; }
    float                       GetWeaponAnimLoopFireTime   ( void )                        { return internalInterface->m_animFireTime; }
    void                        SetWeaponAnimLoopFireTime   ( float animFireTime )          { internalInterface->m_animFireTime = animFireTime; }

    float                       GetWeaponAnim2LoopStart     ( void )                        { return internalInterface->m_anim2LoopStart; }
    float                       GetWeaponAnim2LoopStop      ( void )                        { return internalInterface->m_anim2LoopEnd; }
    float                       GetWeaponAnim2LoopFireTime  ( void )                        { return internalInterface->m_anim2FireTime; }

    float                       GetWeaponAnimBreakoutTime   ( void )                        { return internalInterface->m_animBreakoutTime; }

    float                       GetWeaponRadius             ( void )                        { return internalInterface->m_fRadius; }
    
    // Ints
    short                       GetAimOffsetIndex           ( void )                        { return internalInterface->m_nAimOffsetIndex; }       

    BYTE                        GetDefaultCombo          ( void )                        { return internalInterface->m_defaultCombo; }
    BYTE                        GetCombosAvailable       ( void )                        { return internalInterface->m_nCombosAvailable; }

};

#endif
