/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUIEvent.h
 *  PURPOSE:     Event interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUI.h"
#include "CGUITypes.h"

struct CGUIEventArgs
{
    CGUIElement* pWindow;
};

struct CGUIKeyEventArgs
{
    CGUIElement* pWindow;

    std::uint32_t   codepoint;
    CGUIKeys::Scan scancode;
    std::uint32_t   sysKeys;
};

struct CGUIMouseEventArgs
{
    CGUIElement* pWindow;

    CGUIPosition           position;
    CVector2D              moveDelta;
    CGUIMouse::MouseButton button;
    std::uint32_t           sysKeys;
    float                  wheelChange;
    std::uint32_t           clickCount;
    CGUIElement*           pSwitchedWindow;
};

struct CGUIFocusEventArgs
{
    CGUIElement* pActivatedWindow;
    CGUIElement* pDeactivatedWindow;
};
