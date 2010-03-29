/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/gui/CGUIFont.h
*  PURPOSE:     Font type interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUIFONT_H
#define __CGUIFONT_H

#include "CRect2D.h"

class CGUIFont
{
public:
    virtual                         ~CGUIFont               ( void ) {};

    virtual void                    SetAntiAliasingEnabled  ( bool bAntialiased ) = 0;
    virtual bool                    IsAntiAliasingEnabled   ( void ) = 0;
    virtual void                    SetAutoScalingEnabled   ( bool bAutoScaled ) = 0;
    virtual bool                    IsAutoScalingEnabled    ( void ) = 0;

    virtual void                    SetNativeResolution     ( int iX, int iY ) = 0;

    virtual float                   GetCharacterWidth       ( int iChar, float fScale = 1.0f ) = 0;
    virtual float                   GetFontHeight           ( float fScale = 1.0f ) = 0;
    virtual float                   GetTextExtent           ( const char* szText, float fScale = 1.0f ) = 0;

    virtual void                    DrawTextString          ( const char *szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX, float fScaleY ) = 0;
};

#endif
