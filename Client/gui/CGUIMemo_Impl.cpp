/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIMemo_Impl.cpp
 *  PURPOSE:     Multi-line edit box widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUIMEMO_NAME "CGUI/MultiLineEditbox"

CGUIMemo_Impl::CGUIMemo_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, const char* szEdit)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUIMEMO_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    // m_pWindow->setRect ( CEGUI::Relative, CEGUI::Rect (0.00f, 0.00f, 0.40f, 0.40f ) );

    // Register our events
    m_pWindow->subscribeEvent(CEGUI::MultiLineEditbox::EventTextChanged, CEGUI::Event::Subscriber(&CGUIMemo_Impl::Event_TextChanged, this));
    m_pWindow->subscribeEvent(CEGUI::MultiLineEditbox::EventKeyDown, CEGUI::Event::Subscriber(&CGUIMemo_Impl::Event_OnKeyDown, this));
    AddEvents();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
    {
        SetParent(pParent);
        if (CGUITabList* pTabList = dynamic_cast<CGUITabList*>(pParent))
        {
            pTabList->AddItem(this);
        }
    }
    else
    {
        pGUI->AddChild(this);
        pGUI->AddItem(this);
        SetParent(NULL);
    }
}

CGUIMemo_Impl::~CGUIMemo_Impl()
{
    if (GetParent() == NULL)
    {
        m_pManager->RemoveItem(this);
    }
    else if (CGUITabList* pTabList = dynamic_cast<CGUITabList*>(GetParent()))
    {
        pTabList->RemoveItem(this);
    }
    DestroyElement();
}

void CGUIMemo_Impl::SetReadOnly(bool bReadOnly)
{
    reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->setReadOnly(bReadOnly);
}

bool CGUIMemo_Impl::IsReadOnly()
{
    return reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->isReadOnly();
}

void CGUIMemo_Impl::EnsureCaratIsVisible()
{
    CEGUI::MultiLineEditbox* wndMemo = reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow);
    wndMemo->setCaratIndex((wndMemo->getText()).length());
}

unsigned int CGUIMemo_Impl::GetCaretIndex()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->getCaratIndex());
}

void CGUIMemo_Impl::SetCaretIndex(unsigned int uiIndex)
{
    reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->setCaratIndex(uiIndex);
}

float CGUIMemo_Impl::GetVerticalScrollPosition()
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->d_vertScrollbar;
    if (pScrollbar)
    {
        return pScrollbar->getScrollPosition();
    }

    return 0.0f;
}

void CGUIMemo_Impl::SetVerticalScrollPosition(float fPosition)
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->d_vertScrollbar;
    if (pScrollbar)
    {
        pScrollbar->setScrollPosition(fPosition);
    }
}

float CGUIMemo_Impl::GetMaxVerticalScrollPosition()
{
    return GetScrollbarDocumentSize() - GetScrollbarPageSize();
}

float CGUIMemo_Impl::GetScrollbarDocumentSize()
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->d_vertScrollbar;
    if (pScrollbar)
    {
        return pScrollbar->getDocumentSize();
    }

    return 1.0f;
}

float CGUIMemo_Impl::GetScrollbarPageSize()
{
    CEGUI::Scrollbar* pScrollbar = reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->d_vertScrollbar;
    if (pScrollbar)
    {
        return pScrollbar->getPageSize();
    }

    return 1.0f;
}

void CGUIMemo_Impl::SetSelection(unsigned int uiStart, unsigned int uiEnd)
{
    reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->setSelection(uiStart, uiEnd);
}

unsigned int CGUIMemo_Impl::GetSelectionStart()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->getSelectionStartIndex());
}

unsigned int CGUIMemo_Impl::GetSelectionEnd()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->getSelectionEndIndex());
}

unsigned int CGUIMemo_Impl::GetSelectionLength()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::MultiLineEditbox*>(m_pWindow)->getSelectionLength());
}

bool CGUIMemo_Impl::ActivateOnTab()
{
    // Only select this as active if its visible and writable
    if (IsVisible() && !IsReadOnly())
    {
        Activate();
        SetCaretIndex(GetText().length());
        return true;
    }
    return false;
}

void CGUIMemo_Impl::SetTextChangedHandler(const GUI_CALLBACK& Callback)
{
    m_TextChanged = Callback;
}

bool CGUIMemo_Impl::Event_TextChanged(const CEGUI::EventArgs& e)
{
    if (m_TextChanged)
        m_TextChanged(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIMemo_Impl::Event_OnKeyDown(const CEGUI::EventArgs& e)
{
    const CEGUI::KeyEventArgs& KeyboardArgs = reinterpret_cast<const CEGUI::KeyEventArgs&>(e);
    if (KeyboardArgs.scancode == CGUIKeys::Tab)
    {
        // tab pressed, if we are in a window with tab enabled, just switch to the next element
        if (GetParent() == NULL)
        {
            m_pManager->SelectNext(this);
        }
        else if (CGUITabList* pTabList = dynamic_cast<CGUITabList*>(GetParent()))
        {
            pTabList->SelectNext(this);
        }
    }
    return true;
}
