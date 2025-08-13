/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIFont.h
 *  PURPOSE:     Font type interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CRect2D.h"

class CGUIFont
{
public:
    virtual ~CGUIFont(){};

    virtual void SetAntiAliasingEnabled(bool bAntialiased) = 0;
    virtual bool IsAntiAliasingEnabled() = 0;
    virtual void SetAutoScalingEnabled(bool bAutoScaled) = 0;
    virtual bool IsAutoScalingEnabled() = 0;

    virtual void SetNativeResolution(int iX, int iY) = 0;

    virtual float GetCharacterWidth(int iChar, float fScale = 1.0f) = 0;
    virtual float GetFontHeight(float fScale = 1.0f) = 0;
    virtual float GetTextExtent(const char* szText, float fScale = 1.0f) = 0;

    virtual void DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX,
                                float fScaleY) = 0;
};
