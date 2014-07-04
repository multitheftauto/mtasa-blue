/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponStatSA.h
*  PURPOSE:     Source file for custom weapon stats.
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*****************************************************************************/

#ifndef __CWEAPONSTATSA_H
#define __CWEAPONSTATSA_H
#include "CWeaponInfoSA.h"
class CWeaponStatSA : public CWeaponStat
{
public:
                                CWeaponStatSA                 ( void ){};
                                CWeaponStatSA                 ( eWeaponType weaponType, eWeaponSkill skillLevel );
                                CWeaponStatSA                 ( CWeaponInfoSA * pWeaponInfo, eWeaponType weaponType, eWeaponSkill skillLevel );
                                ~CWeaponStatSA                ( void );

        eWeaponType             GetWeaponType               ( void );
        eWeaponSkill            GetWeaponSkillLevel         ( void );

        void                    SetWeaponType               ( eWeaponType weaponType );
        void                    SetWeaponSkillLevel         ( eWeaponSkill skillLevel );

        //
        // Interface Sets and Gets
        //
        
        void                        SetFlag                 ( DWORD flag )                  { pWeaponStats->m_nFlags |= flag; }
        // DO NOT USE.
        void                        SetFlags                ( int iFlags )                  { pWeaponStats->m_nFlags = iFlags; }
        // END
        void                        ClearFlag               ( DWORD flag )                  { pWeaponStats->m_nFlags &= ~flag; }
        bool                        IsFlagSet               ( DWORD flag )                  { return ((pWeaponStats->m_nFlags & flag) > 0 ? true : false); }
        int                         GetFlags                ( void )                        { return pWeaponStats->m_nFlags; }

        eWeaponModel                GetModel                ( void )                        { return (eWeaponModel)pWeaponStats->m_modelId; }
        void                        SetModel                ( int iModel )                  { pWeaponStats->m_modelId = (int)iModel; }

        eWeaponModel                GetModel2               ( void )                        { return (eWeaponModel)pWeaponStats->m_modelId2; }
        void                        SetModel2               ( int iModel )                  { pWeaponStats->m_modelId2 = (int)iModel; }

        CWeaponInfoSAInterface *    GetInterface            ( void )                        { return pWeaponStats; };

        float                       GetWeaponRange          ( void )                        { return pWeaponStats->m_fWeaponRange; };
        void                        SetWeaponRange          ( float fRange )                { pWeaponStats->m_fWeaponRange = fRange; };

        float                       GetTargetRange          ( void )                        { return pWeaponStats->m_fTargetRange; };
        void                        SetTargetRange          ( float fRange )                { pWeaponStats->m_fTargetRange = fRange; };

        CVector *                   GetFireOffset           ( void )                        { return &pWeaponStats->m_vecFireOffset; };
        void                        SetFireOffset           ( CVector * vecFireOffset )     { pWeaponStats->m_vecFireOffset = *vecFireOffset; };

        short                       GetDamagePerHit         ( void )                        { return pWeaponStats->m_nDamage; };
        void                        SetDamagePerHit         ( short sDamagePerHit )         { pWeaponStats->m_nDamage = sDamagePerHit; };

        float                       GetAccuracy             ( void )                        { return pWeaponStats->m_fAccuracy; };
        void                        SetAccuracy             ( float fAccuracy )             { pWeaponStats->m_fAccuracy = fAccuracy; };

        short                       GetMaximumClipAmmo      ( void )                        { return pWeaponStats->m_nAmmo; };
        void                        SetMaximumClipAmmo      ( short sAccuracy )             { pWeaponStats->m_nAmmo = sAccuracy; };

        float                       GetMoveSpeed            ( void )                        { return pWeaponStats->m_fMoveSpeed; };
        void                        SetMoveSpeed            ( float fMoveSpeed )            { pWeaponStats->m_fMoveSpeed = fMoveSpeed; };

        // projectile/areaeffect only
        float                       GetFiringSpeed          ( void )                        { return pWeaponStats->m_fSpeed; };
        void                        SetFiringSpeed          ( float fFiringSpeed )          { pWeaponStats->m_fSpeed = fFiringSpeed; };

        // area effect only
        float                       GetRadius               ( void )                        { return pWeaponStats->m_fRadius; };
        void                        SetRadius               ( float fRadius )               { pWeaponStats->m_fRadius = fRadius; };

        float                       GetLifeSpan             ( void )                        { return pWeaponStats->m_fLifeSpan; };
        void                        SetLifeSpan             ( float fLifeSpan )             { pWeaponStats->m_fLifeSpan = fLifeSpan; };

        float                       GetSpread               ( void )                        { return pWeaponStats->m_fSpread; };
        void                        SetSpread               ( float fSpread )               { pWeaponStats->m_fSpread = fSpread; };

        float                       GetAnimBreakoutTime     ( void )                        { return pWeaponStats->m_animBreakoutTime; };
        void                        SetAnimBreakoutTime     ( float fBreakoutTime )         { pWeaponStats->m_animBreakoutTime = fBreakoutTime; };

        eWeaponSlot                 GetSlot                 ( void )                        { return (eWeaponSlot)pWeaponStats->m_nWeaponSlot; };
        void                        SetSlot                 ( eWeaponSlot dwSlot )          { pWeaponStats->m_nWeaponSlot = (eWeaponSlot)dwSlot; };

        eWeaponSkill                GetSkill                ( void )                        { return pWeaponStats->m_SkillLevel; }
        void                        SetSkill                ( eWeaponSkill weaponSkill )    { pWeaponStats->m_SkillLevel = weaponSkill; }

        float                       GetRequiredStatLevel    ( void )                        { return static_cast < float > ( pWeaponStats->m_nReqStatLevel ); }
        void                        SetRequiredStatLevel    ( float fStatLevel )            { pWeaponStats->m_nReqStatLevel = static_cast < int > ( fStatLevel ); }
        void                        SetRequiredStatLevel    ( int iStatLevel )              { pWeaponStats->m_nReqStatLevel = iStatLevel; }

        DWORD                       GetAnimGroup            ( void )                        { return pWeaponStats->m_animGroup; }
        void                        SetAnimGroup            ( DWORD dwAnimGroup )           { pWeaponStats->m_animGroup = dwAnimGroup; }

        eFireType                   GetFireType             ( void )                        { return pWeaponStats->m_eFireType; }        
        void                        SetFireType             ( eFireType type )              { pWeaponStats->m_eFireType = type; }

        // Floats
        float                       GetWeaponAnimLoopStart      ( void )                     { return pWeaponStats->m_animLoopStart; }
        void                        SetWeaponAnimLoopStart      ( float animLoopStart )      { pWeaponStats->m_animLoopStart = animLoopStart; }

        float                       GetWeaponAnimLoopStop       ( void )                     { return pWeaponStats->m_animLoopEnd; }
        void                        SetWeaponAnimLoopStop       ( float animLoopEnd )        { pWeaponStats->m_animLoopEnd = animLoopEnd; }

        float                       GetWeaponAnimLoopFireTime   ( void )                     { return pWeaponStats->m_animFireTime; }
        void                        SetWeaponAnimLoopFireTime   ( float animFireTime )       { pWeaponStats->m_animFireTime = animFireTime; }

        float                       GetWeaponAnim2LoopStart     ( void )                     { return pWeaponStats->m_anim2LoopStart; }
        void                        SetWeaponAnim2LoopStart     ( float anim2LoopStart )     { pWeaponStats->m_anim2LoopStart = anim2LoopStart; }

        float                       GetWeaponAnim2LoopStop      ( void )                     { return pWeaponStats->m_anim2LoopEnd; }
        void                        SetWeaponAnim2LoopStop      ( float anim2LoopEnd )       { pWeaponStats->m_anim2LoopEnd = anim2LoopEnd; }

        float                       GetWeaponAnim2LoopFireTime  ( void )                     { return pWeaponStats->m_anim2FireTime; }
        void                        SetWeaponAnim2LoopFireTime  ( float anim2FireTime )      { pWeaponStats->m_anim2FireTime = anim2FireTime; }

        float                       GetWeaponAnimBreakoutTime   ( void )                     { return pWeaponStats->m_animBreakoutTime; }
        void                        SetWeaponAnimBreakoutTime   ( float animBreakoutTime )   { pWeaponStats->m_animBreakoutTime = animBreakoutTime; }

        float                       GetWeaponSpeed              ( void )                     { return pWeaponStats->m_fSpeed; }
        void                        SetWeaponSpeed              ( float fSpeed )             { pWeaponStats->m_fSpeed = fSpeed; }

        float                       GetWeaponRadius             ( void )                     { return pWeaponStats->m_fRadius; }
        void                        SetWeaponRadius             ( float fRadius )            { pWeaponStats->m_fRadius = fRadius; }
        
        // Ints
        short                       GetAimOffsetIndex           ( void )                     { return pWeaponStats->m_nAimOffsetIndex; }    
        void                        SetAimOffsetIndex           ( short sIndex )             { pWeaponStats->m_nAimOffsetIndex = sIndex; }    

        BYTE                        GetDefaultCombo          ( void )                        { return pWeaponStats->m_defaultCombo; }
        void                        SetDefaultCombo          ( BYTE defaultCombo )           { pWeaponStats->m_defaultCombo = defaultCombo; }

        BYTE                        GetCombosAvailable       ( void )                        { return pWeaponStats->m_nCombosAvailable; }
        void                        SetCombosAvailable       ( BYTE nCombosAvailable )       { pWeaponStats->m_nCombosAvailable = nCombosAvailable; }

private:
        eWeaponType weaponType;
        eWeaponSkill skillLevel;
        CWeaponInfoSAInterface* pWeaponStats;


};
#endif