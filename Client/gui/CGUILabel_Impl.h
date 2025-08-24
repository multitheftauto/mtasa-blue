/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUILabel_Impl.h
 *  PURPOSE:     Label widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUILabel.h>
#include "CGUIElement_Impl.h"

class CGUIFont;

class CGUILabel_Impl : public CGUILabel, public CGUIElement_Impl
{
public:
    CGUILabel_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szText = "");
    ~CGUILabel_Impl();

    void SetText(const char* Text);

    void                SetVerticalAlign(CGUIVerticalAlign eAlign);
    CGUIVerticalAlign   GetVerticalAlign();
    void                SetHorizontalAlign(CGUIHorizontalAlign eAlign);
    CGUIHorizontalAlign GetHorizontalAlign();

    void      SetTextColor(CGUIColor Color);
    void      SetTextColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue);
    CGUIColor GetTextColor();
    void      GetTextColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue);

    void SetFrameEnabled(bool bFrameEnabled);
    bool IsFrameEnabled();

    float GetCharacterWidth(int iCharIndex);
    float GetFontHeight();
    float GetTextExtent();

    eCGUIType GetType() { return CGUI_LABEL; };

    #define EXCLUDE_SET_TEXT
    #include "CGUIElement_Inc.h"
    #undef EXCLUDE_SET_TEXT
};
