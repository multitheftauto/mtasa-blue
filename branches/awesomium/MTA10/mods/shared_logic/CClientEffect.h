/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientEffect.h
*  PURPOSE:     Effect entity
*
*****************************************************************************/

#ifndef __CCLIENTEFFECT_H
#define __CCLIENTEFFECT_H

#include <game/CFxSystem.h>
#include "CClientEntity.h"

class CClientManager;

class CClientEffect : public CClientEntity
{
    DECLARE_CLASS( CClientEffect, CClientEntity )
public:
            CClientEffect ( CClientManager * pManager, CFxSystem * pFx, SString strEffectName, ElementID ID );
            ~CClientEffect ( void );
    void    Unlink( void );
    
    void    GetPosition( CVector& vecPosition ) const;
    void    SetPosition( const CVector& vecPosition );
    void    GetRotationRadians( CVector& vecRotation ) const;
    void    SetRotationRadians( const CVector& vecRotation );

    bool    GetMatrix(CMatrix &matrix) const;
    bool    SetMatrix(const CMatrix &matrix);
    void    SetEffectSpeed( float fSpeed );
    float   GetEffectSpeed( void ) const;
    bool    SetEffectDensity( float fDensity );
    float   GetEffectDensity( void ) const;
    void    SetDrawDistance( float fDrawDistance );
    float   GetDrawDistance( void ) const;

    inline eClientEntityType GetType ( void ) const     { return CCLIENTEFFECT; };
protected:
    friend class CClientEffectManager;
    inline CFxSystem*        GetFxSystem( void ) const { return m_pFxSystem; }
    inline void              SetFxSystem ( CFxSystem* pFx ) { m_pFxSystem = pFx; }

private:
    
    CClientManager *        m_pManager;
    CFxSystem *             m_pFxSystem;
    SString                 m_strEffectName;
    float                   m_fMaxDensity;
};

#endif