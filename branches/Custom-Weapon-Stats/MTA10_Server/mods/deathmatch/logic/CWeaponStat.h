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
        void                        ClearFlag               ( DWORD flag )                  { tWeaponStats.m_nFlags &= ~flag; }
        bool                        IsFlagSet               ( DWORD flag )                  { return ((tWeaponStats.m_nFlags & flag) > 0 ? true : false); }

        eWeaponModel                GetModel()                                              { return (eWeaponModel)tWeaponStats.m_modelId; }

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

        DWORD                       GetAnimGroup            ( void )                        { return tWeaponStats.m_animGroup; }
        void                        SetAnimGroup            ( DWORD dwAnimGroup )           { tWeaponStats.m_animGroup = dwAnimGroup; }

        eFireType                   GetFireType             ( void )                        { return tWeaponStats.m_eFireType; }

private:
        eWeaponType weaponType;
        eWeaponSkill skillLevel;
        sWeaponStats tWeaponStats;


};
#endif