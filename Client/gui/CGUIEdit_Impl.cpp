/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIEdit_Impl.cpp
 *  PURPOSE:     Edit box widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUIEDIT_NAME "CGUI/Editbox"

CGUIEdit_Impl::CGUIEdit_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, const char* szEdit)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the edit and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUIEDIT_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);
    m_pWindow->setRect(CEGUI::Absolute, CEGUI::Rect(0.00f, 0.00f, 0.128f, 0.24f));

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    // Register our event
    m_pWindow->subscribeEvent(CEGUI::Editbox::EventKeyDown, CEGUI::Event::Subscriber(&CGUIEdit_Impl::Event_OnKeyDown, this));
    m_pWindow->subscribeEvent(CEGUI::Editbox::EventTextChanged, CEGUI::Event::Subscriber(&CGUIEdit_Impl::Event_OnTextChanged, this));
    m_pWindow->subscribeEvent(CEGUI::Editbox::EventRenderingEnded, CEGUI::Event::Subscriber(&CGUIEdit_Impl::Event_OnRenderingEnded, this));
    m_pWindow->subscribeEvent(CEGUI::Editbox::EventRenderingStarted, CEGUI::Event::Subscriber(&CGUIEdit_Impl::Event_OnRenderingStarted, this));
    AddEvents();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
    {
        SetParent(reinterpret_cast<CGUIElement_Impl*>(pParent));
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

CGUIEdit_Impl::~CGUIEdit_Impl()
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

void CGUIEdit_Impl::SetReadOnly(bool bReadOnly)
{
    reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setReadOnly(bReadOnly);
}

bool CGUIEdit_Impl::IsReadOnly()
{
    return reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->isReadOnly();
}

void CGUIEdit_Impl::SetMasked(bool bMasked)
{
    reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setTextMasked(bMasked);
}

bool CGUIEdit_Impl::IsMasked()
{
    return reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->isTextMasked();
}

void CGUIEdit_Impl::SetMaxLength(unsigned int uiMaxLength)
{
    reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setMaxTextLength(uiMaxLength);
}

unsigned int CGUIEdit_Impl::GetMaxLength()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->getMaxTextLength());
}

void CGUIEdit_Impl::SetSelection(unsigned int uiStart, unsigned int uiEnd)
{
    reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setSelection(uiStart, uiEnd);
}

unsigned int CGUIEdit_Impl::GetSelectionStart()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->getSelectionStartIndex());
}

unsigned int CGUIEdit_Impl::GetSelectionEnd()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->getSelectionEndIndex());
}

unsigned int CGUIEdit_Impl::GetSelectionLength()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->getSelectionLength());
}

void CGUIEdit_Impl::SetCaretIndex(unsigned int uiIndex)
{
    return reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setCaratIndex(uiIndex);
}

void CGUIEdit_Impl::SetCaretAtStart()
{
    reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setCaratIndex(0);
}

void CGUIEdit_Impl::SetCaretAtEnd()
{
    reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->setCaratIndex(GetText().length());
}

unsigned int CGUIEdit_Impl::GetCaretIndex()
{
    return static_cast<unsigned int>(reinterpret_cast<CEGUI::Editbox*>(m_pWindow)->getCaratIndex());
}

void CGUIEdit_Impl::SetTextAcceptedHandler(GUI_CALLBACK Callback)
{
    m_OnTextAccepted = Callback;
}

void CGUIEdit_Impl::SetTextChangedHandler(GUI_CALLBACK Callback)
{
    m_OnTextChanged = Callback;
}

void CGUIEdit_Impl::SetRenderingEndedHandler(GUI_CALLBACK Callback)
{
    m_OnRenderingEnded = Callback;
}

void CGUIEdit_Impl::SetRenderingStartedHandler(GUI_CALLBACK Callback)
{
    m_OnRenderingStarted = Callback;
}

bool CGUIEdit_Impl::ActivateOnTab()
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

bool CGUIEdit_Impl::Event_OnTextChanged(const CEGUI::EventArgs& e)
{
    if (m_OnTextChanged)
        m_OnTextChanged(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIEdit_Impl::Event_OnRenderingEnded(const CEGUI::EventArgs& e)
{
    if (m_OnRenderingEnded)
        m_OnRenderingEnded(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIEdit_Impl::Event_OnRenderingStarted(const CEGUI::EventArgs& e)
{
    if (m_OnRenderingStarted)
        m_OnRenderingStarted(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIEdit_Impl::Event_OnKeyDown(const CEGUI::EventArgs& e)
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
    else if (KeyboardArgs.scancode == CGUIKeys::Return || KeyboardArgs.scancode == CGUIKeys::NumpadEnter)
    {
        // Enter/Return event is split from Tab now, since we use that for Console, Quick Connect, etc. as enter-only
        if (m_OnTextAccepted)
            m_OnTextAccepted(reinterpret_cast<CGUIElement*>(this));
    }
    return true;
}
