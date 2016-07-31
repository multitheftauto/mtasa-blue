/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTextDisplay.h
*  PURPOSE:     Client text display base class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

class CClientTextDisplay;

#ifndef __CCLIENTTEXTDISPLAY_H
#define __CCLIENTTEXTDISPLAY_H

#include "CClientDisplay.h"
#include "CClientDisplayManager.h"
#include <gui/CGUI.h>

class CClientTextDisplay: public CClientDisplay
{
    friend class CClientDisplayManager;

public:
                                CClientTextDisplay      ( CClientDisplayManager* pDisplayManager, int ID = 0xFFFFFFFF );
                                ~CClientTextDisplay     ( void );

    inline eDisplayType         GetType                 ( void )                                { return DISPLAY_TEXT; }

    void                        SetCaption              ( const char* szCaption );

    void                        SetPosition             ( const CVector& vecPosition );

    void                        SetColor                ( const SColor color );
    void                        SetColorAlpha           ( unsigned char ucAlpha );
    void                        SetShadowAlpha          ( unsigned char ucShadowAlpha );

    inline float                GetScale                ( void )                                { return m_fScale; };
    void                        SetScale                ( float fScale );

    inline unsigned long        GetFormat               ( void )                                { return m_ulFormat; };
    void                        SetFormat               ( unsigned long ulFormat );

    void                        SetVisible              ( bool bVisible );

    void                        Render                  ( void );

    static void                 SetGlobalScale          ( float fScale )                        { m_fGlobalScale = fScale; }
private:
    SString                     m_strCaption;
    float                       m_fScale;

    unsigned long               m_ulFormat;
    unsigned char               m_ucShadowAlpha;

    static float                m_fGlobalScale;
};

#endif
