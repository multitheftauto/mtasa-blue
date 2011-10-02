/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponStat.h
*  PURPOSE:     Source file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CWEAPONSTAT_H
#define __CWEAPONSTAT_H
#include <logic/CCommon.h>
struct sWeaponStats
{
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
class CWeaponStat
{
public:
                                CWeaponStat                 ( void ){};
                                CWeaponStat                 ( eWeaponType weaponType, eWeaponSkill skillLevel );
                                ~CWeaponStat                ( void );

        eWeaponType             GetWeaponType               ( void );
        eWeaponSkill            GetWeaponSkillLevel         ( void );

        void                    SetWeaponType               ( eWeaponType weaponType );
        void                    SetWeaponSkillLevel         ( eWeaponSkill skillLevel );

        //
        // Interface Sets and Gets
        //
        
        void                        SetFlag                 ( DWORD flag )                  { tWeaponStats.m_nFlags |= flag; }
        // DO NOT USE.
        void                        SetFlags                ( int iFlags )                  { tWeaponStats.m_nFlags = iFlags; }
        // END
        void                        ClearFlag               ( DWORD flag )                  { tWeaponStats.m_nFlags &= ~flag; }
        bool                        IsFlagSet               ( DWORD flag )                  { return ((tWeaponStats.m_nFlags & flag) > 0 ? true : false); }

        eWeaponModel                GetModel                ( void )                        { return (eWeaponModel)tWeaponStats.m_modelId; }
        void                        SetModel                ( int iModel )                  { tWeaponStats.m_modelId = (int)iModel; }

        eWeaponModel                GetModel2               ( void )                        { return (eWeaponModel)tWeaponStats.m_modelId2; }
        void                        SetModel2               ( int iModel )                  { tWeaponStats.m_modelId2 = (int)iModel; }

        sWeaponStats *              GetInterface            ( void )                        { return &tWeaponStats; };

        float                       GetWeaponRange          ( void )                        { return tWeaponStats.m_fWeaponRange; };
        void                        SetWeaponRange          ( float fRange )                { tWeaponStats.m_fWeaponRange = fRange; };

        float                       GetTargetRange          ( void )                        { return tWeaponStats.m_fTargetRange; };
        void                        SetTargetRange          ( float fRange )                { tWeaponStats.m_fTargetRange = fRange; };

        CVector *                   GetFireOffset           ( void )                        { return &tWeaponStats.m_vecFireOffset; };
        void                        SetFireOffset           ( CVector * vecFireOffset )     { tWeaponStats.m_vecFireOffset = *vecFireOffset; };

        short                       GetDamagePerHit         ( void )                        { return tWeaponStats.m_nDamage; };
        void                        SetDamagePerHit         ( short sDamagePerHit )         { tWeaponStats.m_nDamage = sDamagePerHit; };

        float                       GetAccuracy             ( void )                        { return tWeaponStats.m_fAccuracy; };
        void                        SetAccuracy             ( float fAccuracy )             { tWeaponStats.m_fAccuracy = fAccuracy; };

        short                       GetMaximumClipAmmo      ( void )                        { return tWeaponStats.m_nAmmo; };
        void                        SetMaximumClipAmmo      ( short sAccuracy )             { tWeaponStats.m_nAmmo = sAccuracy; };

        float                       GetMoveSpeed            ( void )                        { return tWeaponStats.m_fMoveSpeed; };
        void                        SetMoveSpeed            ( float fMoveSpeed )            { tWeaponStats.m_fMoveSpeed = fMoveSpeed; };

        // projectile/areaeffect only
        float                       GetFiringSpeed          ( void )                        { return tWeaponStats.m_fSpeed; };
        void                        SetFiringSpeed          ( float fFiringSpeed )          { tWeaponStats.m_fSpeed = fFiringSpeed; };

        // area effect only
        float                       GetRadius               ( void )                        { return tWeaponStats.m_fRadius; };
        void                        SetRadius               ( float fRadius )               { tWeaponStats.m_fRadius = fRadius; };

        float                       GetLifeSpan             ( void )                        { return tWeaponStats.m_fLifeSpan; };
        void                        SetLifeSpan             ( float fLifeSpan )             { tWeaponStats.m_fLifeSpan = fLifeSpan; };

        float                       GetSpread               ( void )                        { return tWeaponStats.m_fSpread; };
        void                        SetSpread               ( float fSpread )               { tWeaponStats.m_fSpread = fSpread; };

        float                       GetAnimBreakoutTime     ( void )                        { return tWeaponStats.m_animBreakoutTime; };
        void                        SetAnimBreakoutTime     ( float fBreakoutTime )         { tWeaponStats.m_animBreakoutTime = fBreakoutTime; };

        eWeaponSlot                 GetSlot                 ( void )                        { return (eWeaponSlot)tWeaponStats.m_nWeaponSlot; };
        void                        SetSlot                 ( eWeaponSlot dwSlot )          { tWeaponStats.m_nWeaponSlot = (eWeaponSlot)dwSlot; };

        eWeaponSkill                GetSkill                ( void )                        { return tWeaponStats.m_SkillLevel; }
        void                        SetSkill                ( eWeaponSkill weaponSkill )    { tWeaponStats.m_SkillLevel = weaponSkill; }

        float                       GetRequiredStatLevel    ( void )                        { return static_cast < float > ( tWeaponStats.m_nReqStatLevel ); }
        void                        SetRequiredStatLevel    ( float fStatLevel )            { tWeaponStats.m_nReqStatLevel = static_cast < int > ( fStatLevel ); }
        void                        SetRequiredStatLevel    ( int iStatLevel )              { tWeaponStats.m_nReqStatLevel = iStatLevel; }

        DWORD                       GetAnimGroup            ( void )                        { return tWeaponStats.m_animGroup; }
        void                        SetAnimGroup            ( DWORD dwAnimGroup )           { tWeaponStats.m_animGroup = dwAnimGroup; }

        eFireType                   GetFireType             ( void )                        { return tWeaponStats.m_eFireType; }        
        void                        SetFireType             ( eFireType type )              { tWeaponStats.m_eFireType = type; }

        // Floats
        float                       GetWeaponAnimLoopStart      ( void )                     { return tWeaponStats.m_animLoopStart; }
        void                        SetWeaponAnimLoopStart      ( float animLoopStart )      { tWeaponStats.m_animLoopStart = animLoopStart; }

        float                       GetWeaponAnimLoopStop       ( void )                     { return tWeaponStats.m_animLoopEnd; }
        void                        SetWeaponAnimLoopStop       ( float animLoopEnd )        { tWeaponStats.m_animLoopEnd = animLoopEnd; }

        float                       GetWeaponAnimLoopFireTime   ( void )                     { return tWeaponStats.m_animFireTime; }
        void                        SetWeaponAnimLoopFireTime   ( float animFireTime )       { tWeaponStats.m_animFireTime = animFireTime; }

        float                       GetWeaponAnim2LoopStart     ( void )                     { return tWeaponStats.m_anim2LoopStart; }
        void                        SetWeaponAnim2LoopStart     ( float anim2LoopStart )     { tWeaponStats.m_anim2LoopStart = anim2LoopStart; }

        float                       GetWeaponAnim2LoopStop      ( void )                     { return tWeaponStats.m_anim2LoopEnd; }
        void                        SetWeaponAnim2LoopStop      ( float anim2LoopEnd )       { tWeaponStats.m_anim2LoopEnd = anim2LoopEnd; }

        float                       GetWeaponAnim2LoopFireTime  ( void )                     { return tWeaponStats.m_anim2FireTime; }
        void                        SetWeaponAnim2LoopFireTime  ( float anim2FireTime )      { tWeaponStats.m_anim2FireTime = anim2FireTime; }

        float                       GetWeaponAnimBreakoutTime   ( void )                     { return tWeaponStats.m_animBreakoutTime; }
        void                        SetWeaponAnimBreakoutTime   ( float animBreakoutTime )   { tWeaponStats.m_animBreakoutTime = animBreakoutTime; }

        float                       GetWeaponSpeed              ( void )                     { return tWeaponStats.m_fSpeed; }
        void                        SetWeaponSpeed              ( float fSpeed )             { tWeaponStats.m_fSpeed = fSpeed; }

        float                       GetWeaponRadius             ( void )                     { return tWeaponStats.m_fRadius; }
        void                        SetWeaponRadius             ( float fRadius )            { tWeaponStats.m_fRadius = fRadius; }
        
        // Ints
        short                       GetAimOffsetIndex           ( void )                     { return tWeaponStats.m_nAimOffsetIndex; }    
        void                        SetAimOffsetIndex           ( short sIndex )             { tWeaponStats.m_nAimOffsetIndex = sIndex; }    

        BYTE                        GetDefaultCombo          ( void )                        { return tWeaponStats.m_defaultCombo; }
        void                        SetDefaultCombo          ( BYTE defaultCombo )           { tWeaponStats.m_defaultCombo = defaultCombo; }

        BYTE                        GetCombosAvailable       ( void )                        { return tWeaponStats.m_nCombosAvailable; }
        void                        SetCombosAvailable       ( BYTE nCombosAvailable )       { tWeaponStats.m_nCombosAvailable = nCombosAvailable; }

private:
        eWeaponType weaponType;
        eWeaponSkill skillLevel;
        sWeaponStats tWeaponStats;


};
#endif