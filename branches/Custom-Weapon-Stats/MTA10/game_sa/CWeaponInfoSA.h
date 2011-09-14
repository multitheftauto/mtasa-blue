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
    CWeaponInfoSA(CWeaponInfoSAInterface * weaponInfoInterface, eWeaponType weaponType) { this->internalInterface = weaponInfoInterface;  this->weaponType = weaponType; };

    // stolen from R*'s CPedIK :)
    void SetFlag(DWORD flag)                                                { this->internalInterface->m_nFlags |= flag; }
    void ClearFlag(DWORD flag)                                              { this->internalInterface->m_nFlags &= ~flag; }
    bool IsFlagSet(DWORD flag)                                              { return ((this->internalInterface->m_nFlags & flag) > 0 ? 1 : 0); }  

    eWeaponModel                GetModel()                                  { return (eWeaponModel)this->internalInterface->m_modelId; }
    CWeaponInfoSAInterface      * GetInterface( void )                      { return this->internalInterface; };
    float                       GetWeaponRange( void )                      { return GetInterface()->m_fWeaponRange; };
    void                        SetWeaponRange ( FLOAT fRange )             { GetInterface()->m_fWeaponRange = fRange; };
    float                       GetTargetRange( void )                      { return GetInterface()->m_fTargetRange; };
    void                        SetTargetRange ( float fRange )             { GetInterface()->m_fTargetRange = fRange; };
    CVector                     * GetFireOffset( void )                     { return &GetInterface()->m_vecFireOffset; };
    void                        SetFireOffset ( CVector * vecFireOffset )   { MemCpyFast (&GetInterface()->m_vecFireOffset, vecFireOffset, sizeof(CVector)); };
    short                       GetDamagePerHit( void )                     { return GetInterface()->m_nDamage; };
    void                        SetDamagePerHit ( short sDamagePerHit )     { GetInterface()->m_nDamage = sDamagePerHit; };
    float                       GetAccuracy ( void )                        { return internalInterface->m_fAccuracy; };
    void                        SetAccuracy ( float fAccuracy )             { internalInterface->m_fAccuracy = fAccuracy; };
    // projectile/areaeffect only
    float                       GetFiringSpeed( void )                      { return GetInterface()->m_fSpeed; };
    void                        SetFiringSpeed ( float fFiringSpeed )       { GetInterface()->m_fSpeed = fFiringSpeed; };
    // area effect only
    float                       GetRadius( void )                           { return GetInterface()->m_fRadius; };
    void                        SetRadius ( float fRadius )                 { GetInterface()->m_fRadius = fRadius; };
    float                       GetLifeSpan( void )                         { return GetInterface()->m_fLifeSpan; };
    void                        SetLifeSpan ( float fLifeSpan )             { GetInterface()->m_fLifeSpan = fLifeSpan; };
    float                       GetSpread( void )                           { return GetInterface()->m_fSpread; };
    void                        SetSpread ( float fSpread )                 { GetInterface()->m_fSpread = fSpread; };
    float                       GetAnimBreakoutTime( void )                 { return GetInterface()->m_animBreakoutTime; };
    void                        SetAnimBreakoutTime ( FLOAT fBreakoutTime ) { GetInterface()->m_animBreakoutTime = fBreakoutTime; };
    eWeaponSlot                 GetSlot( void )                             { return (eWeaponSlot)GetInterface()->m_nWeaponSlot; };
    void                        SetSlot ( eWeaponSlot dwSlot )              { GetInterface()->m_nWeaponSlot = (eWeaponSlot)dwSlot; };
    eWeaponSkill                GetSkill( void )                            { return GetInterface()->m_SkillLevel; }
    void                        SetSkill ( eWeaponSkill weaponSkill )       { GetInterface ()->m_SkillLevel = weaponSkill; }
    float                       GetRequiredStatLevel ( void )               { return static_cast < float > ( GetInterface()->m_nReqStatLevel ); }
    void                        SetRequiredStatLevel ( float fStatLevel )   { GetInterface()->m_nReqStatLevel = static_cast < int > ( fStatLevel ); }
    eFireType                   GetFireType ( void )                        { return GetInterface()->m_eFireType; }
};

#endif
