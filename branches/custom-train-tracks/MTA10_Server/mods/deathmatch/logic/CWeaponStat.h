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
                                CWeaponStat                 ( void );
                                CWeaponStat                 ( eWeaponType weaponType, eWeaponSkill skillLevel );
                                ~CWeaponStat                ( void );

        eWeaponType             GetWeaponType               ( void );
        eWeaponSkill            GetWeaponSkillLevel         ( void );

        void                    SetWeaponType               ( eWeaponType weaponType );
        void                    SetWeaponSkillLevel         ( eWeaponSkill skillLevel );

        //
        // Interface Sets and Gets
        //
        
        void                        SetFlag                 ( DWORD flag )                  { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nFlags |= flag; }
        // DO NOT USE.
        void                        SetFlags                ( int iFlags )                  { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nFlags = iFlags; }
        // END
        void                        ClearFlag               ( DWORD flag )                  { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nFlags &= ~flag; }
        bool                        IsFlagSet               ( DWORD flag )                  { return ((tWeaponStats.m_nFlags & flag) > 0 ? true : false); }
        int                         GetFlags                ( void )                        { return tWeaponStats.m_nFlags; }

        eWeaponModel                GetModel                ( void )                        { return (eWeaponModel)tWeaponStats.m_modelId; }
        void                        SetModel                ( int iModel )                  { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_modelId = (int)iModel; }

        eWeaponModel                GetModel2               ( void )                        { return (eWeaponModel)tWeaponStats.m_modelId2; }
        void                        SetModel2               ( int iModel )                  { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_modelId2 = (int)iModel; }

        sWeaponStats *              GetInterface            ( void )                        { return &tWeaponStats; };

        float                       GetWeaponRange          ( void )                        { return tWeaponStats.m_fWeaponRange; };
        void                        SetWeaponRange          ( float fRange )                { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fWeaponRange = fRange; };

        float                       GetTargetRange          ( void )                        { return tWeaponStats.m_fTargetRange; };
        void                        SetTargetRange          ( float fRange )                { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fTargetRange = fRange; };

        CVector *                   GetFireOffset           ( void )                        { return &tWeaponStats.m_vecFireOffset; };
        void                        SetFireOffset           ( CVector * vecFireOffset )     { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_vecFireOffset = *vecFireOffset; };

        short                       GetDamagePerHit         ( void )                        { return tWeaponStats.m_nDamage; };
        void                        SetDamagePerHit         ( short sDamagePerHit )         { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nDamage = sDamagePerHit; };

        float                       GetAccuracy             ( void )                        { return tWeaponStats.m_fAccuracy; };
        void                        SetAccuracy             ( float fAccuracy )             { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fAccuracy = fAccuracy; };

        short                       GetMaximumClipAmmo      ( void )                        { return tWeaponStats.m_nAmmo; };
        void                        SetMaximumClipAmmo      ( short sAccuracy )             { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nAmmo = sAccuracy; };

        float                       GetMoveSpeed            ( void )                        { return tWeaponStats.m_fMoveSpeed; };
        void                        SetMoveSpeed            ( float fMoveSpeed )            { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fMoveSpeed = fMoveSpeed; };

        // projectile/areaeffect only
        float                       GetFiringSpeed          ( void )                        { return tWeaponStats.m_fSpeed; };
        void                        SetFiringSpeed          ( float fFiringSpeed )          { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fSpeed = fFiringSpeed; };

        // area effect only
        float                       GetRadius               ( void )                        { return tWeaponStats.m_fRadius; };
        void                        SetRadius               ( float fRadius )               { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fRadius = fRadius; };

        float                       GetLifeSpan             ( void )                        { return tWeaponStats.m_fLifeSpan; };
        void                        SetLifeSpan             ( float fLifeSpan )             { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fLifeSpan = fLifeSpan; };

        float                       GetSpread               ( void )                        { return tWeaponStats.m_fSpread; };
        void                        SetSpread               ( float fSpread )               { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fSpread = fSpread; };

        float                       GetAnimBreakoutTime     ( void )                        { return tWeaponStats.m_animBreakoutTime; };
        void                        SetAnimBreakoutTime     ( float fBreakoutTime )         { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_animBreakoutTime = fBreakoutTime; };

        eWeaponSlot                 GetSlot                 ( void )                        { return (eWeaponSlot)tWeaponStats.m_nWeaponSlot; };
        void                        SetSlot                 ( eWeaponSlot dwSlot )          { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nWeaponSlot = (eWeaponSlot)dwSlot; };

        eWeaponSkill                GetSkill                ( void )                        { return tWeaponStats.m_SkillLevel; }
        void                        SetSkill                ( eWeaponSkill weaponSkill )    { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_SkillLevel = weaponSkill; }

        float                       GetRequiredStatLevel    ( void )                        { return static_cast < float > ( tWeaponStats.m_nReqStatLevel ); }
        void                        SetRequiredStatLevel    ( float fStatLevel )            { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nReqStatLevel = static_cast < int > ( fStatLevel ); }
        void                        SetRequiredStatLevel    ( int iStatLevel )              { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nReqStatLevel = iStatLevel; }

        DWORD                       GetAnimGroup            ( void )                        { return tWeaponStats.m_animGroup; }
        void                        SetAnimGroup            ( DWORD dwAnimGroup )           { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_animGroup = dwAnimGroup; }

        eFireType                   GetFireType             ( void )                        { return tWeaponStats.m_eFireType; }        
        void                        SetFireType             ( eFireType type )              { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_eFireType = type; }

        // Floats
        float                       GetWeaponAnimLoopStart      ( void )                     { return tWeaponStats.m_animLoopStart; }
        void                        SetWeaponAnimLoopStart      ( float animLoopStart )      { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_animLoopStart = animLoopStart; }

        float                       GetWeaponAnimLoopStop       ( void )                     { return tWeaponStats.m_animLoopEnd; }
        void                        SetWeaponAnimLoopStop       ( float animLoopEnd )        { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_animLoopEnd = animLoopEnd; }

        float                       GetWeaponAnimLoopFireTime   ( void )                     { return tWeaponStats.m_animFireTime; }
        void                        SetWeaponAnimLoopFireTime   ( float animFireTime )       { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_animFireTime = animFireTime; }

        float                       GetWeaponAnim2LoopStart     ( void )                     { return tWeaponStats.m_anim2LoopStart; }
        void                        SetWeaponAnim2LoopStart     ( float anim2LoopStart )     { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_anim2LoopStart = anim2LoopStart; }

        float                       GetWeaponAnim2LoopStop      ( void )                     { return tWeaponStats.m_anim2LoopEnd; }
        void                        SetWeaponAnim2LoopStop      ( float anim2LoopEnd )       { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_anim2LoopEnd = anim2LoopEnd; }

        float                       GetWeaponAnim2LoopFireTime  ( void )                     { return tWeaponStats.m_anim2FireTime; }
        void                        SetWeaponAnim2LoopFireTime  ( float anim2FireTime )      { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_anim2FireTime = anim2FireTime; }

        float                       GetWeaponAnimBreakoutTime   ( void )                     { return tWeaponStats.m_animBreakoutTime; }
        void                        SetWeaponAnimBreakoutTime   ( float animBreakoutTime )   { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_animBreakoutTime = animBreakoutTime; }

        float                       GetWeaponSpeed              ( void )                     { return tWeaponStats.m_fSpeed; }
        void                        SetWeaponSpeed              ( float fSpeed )             { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fSpeed = fSpeed; }

        float                       GetWeaponRadius             ( void )                     { return tWeaponStats.m_fRadius; }
        void                        SetWeaponRadius             ( float fRadius )            { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_fRadius = fRadius; }
        
        // Ints
        short                       GetAimOffsetIndex           ( void )                     { return tWeaponStats.m_nAimOffsetIndex; }    
        void                        SetAimOffsetIndex           ( short sIndex )             { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nAimOffsetIndex = sIndex; }    

        BYTE                        GetDefaultCombo          ( void )                        { return tWeaponStats.m_defaultCombo; }
        void                        SetDefaultCombo          ( BYTE defaultCombo )           { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_defaultCombo = defaultCombo; }

        BYTE                        GetCombosAvailable       ( void )                        { return tWeaponStats.m_nCombosAvailable; }
        void                        SetCombosAvailable       ( BYTE nCombosAvailable )       { ms_uiAllWeaponStatsRevision++; tWeaponStats.m_nCombosAvailable = nCombosAvailable; }

        void                        SetChanged               ( bool bChanged )                        { m_bChanged = bChanged; }
        bool                        HasChanged               ( void )                                 { return m_bChanged; }

        static uint                 GetAllWeaponStatsRevision ( void )                      { return ms_uiAllWeaponStatsRevision; }
private:
        eWeaponType                 weaponType;
        eWeaponSkill                skillLevel;
        sWeaponStats                tWeaponStats;
        bool                        m_bChanged;
        static uint                 ms_uiAllWeaponStatsRevision;
};
#endif