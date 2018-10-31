/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIWindow_Impl.h
 *  PURPOSE:     Window widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUIWindow.h>
#include "CGUIElement_Impl.h"

class CGUIWindow_Impl : public CGUIWindow, public CGUIElement_Impl, public CGUITabList
{
public:
    CGUIWindow_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL, const char* szCaption = "", const SString& strLayoutFile = "");
    ~CGUIWindow_Impl(void);

    void SetMovable(bool bMovable);
    bool IsMovable(void);
    void SetSizingEnabled(bool bResizeEnabled);
    bool IsSizingEnabled(void);
    void SetFrameEnabled(bool bFrameEnabled);
    bool IsFrameEnabled(void);
    void SetCloseButtonEnabled(bool bCloseButtonEnabled);
    bool IsCloseButtonEnabled(void);
    void SetTitlebarEnabled(bool bTitlebarEnabled);
    bool IsTitlebarEnabled(void);

    void SetCloseClickHandler(GUI_CALLBACK Callback);

    eCGUIType GetType(void) { return CGUI_WINDOW; };

    #include "CGUIElement_Inc.h"

protected:
    bool Event_OnCloseClick(const CEGUI::EventArgs& e);

    GUI_CALLBACK m_OnCloseClick;
};
