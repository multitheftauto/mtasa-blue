/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUILabel.h
 *  PURPOSE:     Label widget interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"

class CGUILabel : public CGUIElement
{
public:
    virtual ~CGUILabel(){};

    virtual void SetText(const char* Text) = 0;

    virtual void                SetVerticalAlign(CGUIVerticalAlign eAlign) = 0;
    virtual CGUIVerticalAlign   GetVerticalAlign() = 0;
    virtual void                SetHorizontalAlign(CGUIHorizontalAlign eAlign) = 0;
    virtual CGUIHorizontalAlign GetHorizontalAlign() = 0;

    virtual void      SetTextColor(CGUIColor Color) = 0;
    virtual void      SetTextColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue) = 0;
    virtual CGUIColor GetTextColor() = 0;
    virtual void      GetTextColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue) = 0;

    virtual void SetFrameEnabled(bool bFrameEnabled) = 0;
    virtual bool IsFrameEnabled() = 0;

    virtual float GetCharacterWidth(int iCharIndex) = 0;
    virtual float GetFontHeight() = 0;
    virtual float GetTextExtent() = 0;
};
