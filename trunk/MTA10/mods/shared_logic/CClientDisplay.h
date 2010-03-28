/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDisplay.h
*  PURPOSE:     Text display class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

class CClientDisplay;

#ifndef __ClientDisplay_H
#define __ClientDisplay_H

#include "CClientTime.h"
#include "CClientCommon.h"

enum eDisplayType
{
    DISPLAY_TEXT,
};

class CClientDisplay
{
    friend class CClientDisplayManager;
public:
                                CClientDisplay    ( class CClientDisplayManager* pDisplayManager, unsigned long ulID );
    virtual                     ~CClientDisplay   ( void );

    inline unsigned long        GetID                   ( void )                                { return m_ulID; }
    virtual eDisplayType        GetType                 ( void ) = 0;

    inline unsigned long        GetExpirationTime       ( void )                                { return m_ulExpirationTime; };
    inline void                 SetExpirationTime       ( unsigned long ulTime )                { m_ulExpirationTime = ulTime; };
    inline unsigned long        GetTimeTillExpiration   ( void )                                { return m_ulExpirationTime - CClientTime::GetTime (); };
    inline void                 SetTimeTillExpiration   ( unsigned long ulMs )                  { m_ulExpirationTime = CClientTime::GetTime () + ulMs; };

    virtual const CVector&      GetPosition             ( void )                                { return m_vecPosition; };
    virtual void                SetPosition             ( const CVector& vecPosition )          { m_vecPosition = vecPosition; };

    virtual SColor              GetColor                ( void ) const                          { return m_Color; }
    virtual void                SetColor                ( const SColor color )                  { m_Color = color; }
    virtual void                SetColorAlpha           ( unsigned char ucAlpha );

    virtual bool                IsVisible               ( void )                                { return m_bVisible; };
    virtual void                SetVisible              ( bool bVisible )                       { m_bVisible = bVisible; };

    virtual void                Render                  ( void ) = 0;

protected:
    inline bool                 IsExpired               ( void )                                { return ( m_ulExpirationTime != 0 && ( CClientTime::GetTime () > m_ulExpirationTime ) ); };

    CClientDisplayManager*      m_pDisplayManager;

    unsigned long               m_ulID;
    unsigned long               m_ulExpirationTime;
    bool                        m_bVisible;
    CVector                     m_vecPosition;
    SColor                      m_Color;
};

#endif
