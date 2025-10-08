/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabPanel_Impl.h
 *  PURPOSE:     Tab panel widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUITabPanel.h>
#include "CGUIElement_Impl.h"
#include "CGUITab_Impl.h"

#define MAX_TABS 64

class CGUITabPanel_Impl : public CGUITabPanel, public CGUIElement_Impl
{
    friend class CGUITab_Impl;

public:
    CGUITabPanel_Impl(class CGUI_Impl* pGUI, CGUIElement* pParent = NULL);
    ~CGUITabPanel_Impl();

    CGUITab* CreateTab(const char* szCaption);
    void     DeleteTab(CGUITab* pTab);

    CGUITab* GetSelectedTab();
    void     SetSelectedTab(CGUITab* pTab);

    void         SetSelectedIndex(unsigned int uiIndex);
    unsigned int GetSelectedIndex();

    unsigned int GetTabCount();

    bool IsTabSelected(CGUITab* pTab);

    eCGUIType GetType() { return CGUI_TABPANEL; };

    void SetSelectionHandler(GUI_CALLBACK Callback);

    #include "CGUIElement_Inc.h"

private:
    class CGUI_Impl* m_pGUI;

    GUI_CALLBACK m_OnSelectionChanged;

    bool Event_OnSelectionChanged(const CEGUI::EventArgs& e);
};
