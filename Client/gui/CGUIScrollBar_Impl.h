/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIScrollBar_Impl.h
 *  PURPOSE:     Scroll bar widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIScrollBar.h>
#include "CGUIElement_Impl.h"

class CGUIScrollBar_Impl : public CGUIScrollBar, public CGUIElement_Impl
{
public:
    CGUIScrollBar_Impl(class CGUI_Impl* pGUI, bool bHorizontal, CGUIElement* pParent = NULL);
    ~CGUIScrollBar_Impl();

    void SetOnScrollHandler(const GUI_CALLBACK& Callback);

    void  SetScrollPosition(float fPosition);
    float GetScrollPosition();

    eCGUIType GetType() { return CGUI_SCROLLBAR; };

    #include "CGUIElement_Inc.h"

private:
    bool Event_OnScroll(const CEGUI::EventArgs& e);

    GUI_CALLBACK m_OnScroll;
};
