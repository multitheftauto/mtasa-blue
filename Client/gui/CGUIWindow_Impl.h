/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIWindow_Impl.h
 *  PURPOSE:     Window widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIWindow.h>
#include "CGUIElement_Impl.h"

class CGUIWindow_Impl : public CGUIWindow, public CGUIElement_Impl, public CGUITabList
{
public:
    CGUIWindow_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "", const SString& strLayoutFile = "");
    ~CGUIWindow_Impl();

    void SetMovable(bool bMovable);
    bool IsMovable();
    void SetSizingEnabled(bool bResizeEnabled);
    bool IsSizingEnabled();
    void SetFrameEnabled(bool bFrameEnabled);
    bool IsFrameEnabled();
    void SetCloseButtonEnabled(bool bCloseButtonEnabled);
    bool IsCloseButtonEnabled();
    void SetTitlebarEnabled(bool bTitlebarEnabled);
    bool IsTitlebarEnabled();

    void SetCloseClickHandler(GUI_CALLBACK Callback);

    eCGUIType GetType() { return CGUI_WINDOW; };

    #include "CGUIElement_Inc.h"

protected:
    bool Event_OnCloseClick(const CEGUI::EventArgs& e);

    GUI_CALLBACK m_OnCloseClick;
};
