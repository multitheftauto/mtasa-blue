/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIScrollPane_Impl.h
 *  PURPOSE:     Scroll pane widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIScrollPane.h>
#include "CGUIElement_Impl.h"

class CGUIScrollPane_Impl : public CGUIScrollPane, public CGUIElement_Impl, public CGUITabList
{
public:
    CGUIScrollPane_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL);
    ~CGUIScrollPane_Impl();

    void SetHorizontalScrollBar(bool bEnabled);
    void SetVerticalScrollBar(bool bEnabled);

    void  SetHorizontalScrollPosition(float fPosition);
    float GetHorizontalScrollPosition();

    void  SetVerticalScrollPosition(float fPosition);
    float GetVerticalScrollPosition();

    void  SetHorizontalScrollStepSize(float fPosition);
    float GetHorizontalScrollStepSize();

    void  SetVerticalScrollStepSize(float fPosition);
    float GetVerticalScrollStepSize();

    eCGUIType GetType() { return CGUI_SCROLLPANE; };

    #include "CGUIElement_Inc.h"

private:
    class CGUI_Impl* m_pGUI;
};
