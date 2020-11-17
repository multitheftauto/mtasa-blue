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

namespace GUINew{
    struct CGUIEventArgs;
    struct CGUIKeyEventArgs;
    struct CGUIMouseEventArgs;
    struct CGUIFocusEventArgs;
}

struct GUINew::CGUIEventArgs
{
    CGUIElement* pWindow;
};

struct GUINew::CGUIKeyEventArgs
{
    CGUIElement* pWindow;

    unsigned int   codepoint;
    CGUIKeys::Scan scancode;
    unsigned int   sysKeys;
};

struct GUINew::CGUIMouseEventArgs
{
    CGUIElement* pWindow;

    CGUIPosition           position;
    CVector2D              moveDelta;
    CGUIMouse::MouseButton button;
    unsigned int           sysKeys;
    float                  wheelChange;
    unsigned int           clickCount;
    CGUIElement*           pSwitchedWindow;
};

struct GUINew::CGUIFocusEventArgs
{
    CGUIElement* pActivatedWindow;
    CGUIElement* pDeactivatedWindow;
};
