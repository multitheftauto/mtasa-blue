/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIButton_Impl.h
 *  PURPOSE:     Button widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui_new/CGUIButton.h>
#include "CGUIElement_Impl.h"

namespace GUINew{
    class CGUIButton_Impl;
}

class GUINew::CGUIButton_Impl : public CGUIButton, public CGUIElement_Impl
{
public:
    CGUIButton_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "");
    ~CGUIButton_Impl();

    eCGUIType GetType() { return GUINew::CGUI_BUTTON; };

    #include "CGUIElement_Inc.h"
};
