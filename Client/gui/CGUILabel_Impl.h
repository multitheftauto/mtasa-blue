/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        gui/CGUILabel_Impl.h
*  PURPOSE:     Label widget class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGUILABEL_IMPL_H
#define __CGUILABEL_IMPL_H

#include <gui/CGUILabel.h>
#include "CGUIElement_Impl.h"

class CGUIFont;

class CGUILabel_Impl : public CGUILabel, public CGUIElement_Impl
{
public:
                            CGUILabel_Impl              ( class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szText = "" );
                            ~CGUILabel_Impl             ( void );

    void                    SetText                     ( const char* Text );

    void                    SetVerticalAlign            ( CGUIVerticalAlign eAlign );
    CGUIVerticalAlign       GetVerticalAlign            ( void );
    void                    SetHorizontalAlign          ( CGUIHorizontalAlign eAlign );
    CGUIHorizontalAlign     GetHorizontalAlign          ( void );

    void                    SetTextColor                ( CGUIColor Color );
    void                    SetTextColor                ( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );
    CGUIColor               GetTextColor                ( void );
    void                    GetTextColor                ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue );

    void                    SetFrameEnabled             ( bool bFrameEnabled );
    bool                    IsFrameEnabled              ( void );

    float                   GetCharacterWidth           ( int iCharIndex );
    float                   GetFontHeight               ( void );
    float                   GetTextExtent               ( void );

    eCGUIType               GetType                     ( void ) { return CGUI_LABEL; };

    #define EXCLUDE_SET_TEXT
    #include "CGUIElement_Inc.h"
    #undef EXCLUDE_SET_TEXT

};

#endif
