/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIScrollBar_Impl.cpp
 *  PURPOSE:     Scroll bar widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUISCROLLBAR_HORIZONTAL_NAME   "CGUI/HorizontalScrollbar"
#define CGUISCROLLBAR_VERTICAL_NAME     "CGUI/VerticalScrollbar"

CGUIScrollBar_Impl::CGUIScrollBar_Impl(CGUI_Impl* pGUI, bool bHorizontal, CGUIElement* pParent)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(bHorizontal ? CGUISCROLLBAR_HORIZONTAL_NAME : CGUISCROLLBAR_VERTICAL_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    // Register our events
    m_pWindow->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&CGUIScrollBar_Impl::Event_OnScroll, this));
    AddEvents();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
    {
        SetParent(pParent);
    }
    else
    {
        pGUI->AddChild(this);
        SetParent(NULL);
    }
}

CGUIScrollBar_Impl::~CGUIScrollBar_Impl()
{
    DestroyElement();
}

void CGUIScrollBar_Impl::SetScrollPosition(float fPosition)
{
    reinterpret_cast<CEGUI::Scrollbar*>(m_pWindow)->setScrollPosition(fPosition);
}

float CGUIScrollBar_Impl::GetScrollPosition()
{
    return reinterpret_cast<CEGUI::Scrollbar*>(m_pWindow)->getScrollPosition();
}

void CGUIScrollBar_Impl::SetOnScrollHandler(const GUI_CALLBACK& Callback)
{
    m_OnScroll = Callback;
}

bool CGUIScrollBar_Impl::Event_OnScroll(const CEGUI::EventArgs& e)
{
    if (m_OnScroll)
        m_OnScroll(reinterpret_cast<CGUIElement*>(this));
    return true;
}
