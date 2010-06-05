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
#include "CClientTextDisplayEffect.h"
#include "CClientTime.h"
#include <gui/CGUI.h>

class CClientTextDisplay: public CClientDisplay
{
    friend CClientDisplayManager;

public:
                                CClientTextDisplay      ( CClientDisplayManager* pDisplayManager, int ID, bool bIs3DTex );
                                ~CClientTextDisplay     ( void );

    inline eDisplayType         GetType                 ( void )                                { return DISPLAY_TEXT; }

    void                        ApplyEffect             ( CClientTextDisplayEffect* pTextDisplayEffect );
    void                        RemoveEffect            ( CClientTextDisplayEffect* pTextDisplayEffect );
    void                        RemoveAllEffects        ( void );

    inline const char*          GetCaptionPointer       ( void )                                { return m_szCaption; };
    char*                       GetCaption              ( char* szBuffer, size_t sizeBuffer );
    void                        SetCaption              ( const char* szCaption );

	void						SetPosition             ( const CVector& vecPosition );

    void						SetColor                ( RGBA rgbaColor );
    void						SetColor                ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha )     { SetColor ( COLOR_RGBA ( ucRed, ucGreen, ucBlue, ucAlpha ) ); };
    void                        SetColorAlpha           ( unsigned char ucAlpha );
    void                        SetShadowAlpha          ( unsigned char ucShadowAlpha );

	inline float                GetScale                ( void )                                { return m_fScale; };
    void                        SetScale                ( float fScale );

    inline unsigned long        GetFormat               ( void )                                { return m_ulFormat; };
    void                        SetFormat               ( unsigned long ulFormat );

    void                        SetVisible              ( bool bVisible );

    void                        Render                  ( bool bPulseEffects );

    static void                 SetGlobalScale          ( float fScale )                        { m_fGlobalScale = fScale; }
private:
    char*                       m_szCaption;
    bool                        m_bIs3DText;
    float                       m_fScale;

    unsigned long               m_ulFormat;
    unsigned char               m_ucShadowAlpha;

    std::list < CClientTextDisplayEffect* > m_EffectList;
    static float                m_fGlobalScale;
};

#endif
