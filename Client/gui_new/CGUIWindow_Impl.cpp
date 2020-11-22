/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIWindow_Impl.cpp
 *  PURPOSE:     Window widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUIWINDOW_NAME "FrameWindow"

using namespace GUINew;

CGUIWindow_Impl::CGUIWindow_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, const char* szCaption, const SString& strLayoutFile)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings

    if (!strLayoutFile.empty())
    {
        // Load from XML file
        m_pWindow = pGUI->GetWindowManager()->loadLayoutFromFile(strLayoutFile);
    }

    if (!m_pWindow)
    {
        // Create new here
        m_pWindow = pGUI->GetWindowManager()->createWindow(pGUI->GetElementPrefix() + "/" + CGUIWINDOW_NAME, szUnique);
        m_pWindow->setArea(CEGUI::URect(CEGUI::UDim(0.10f, 0), CEGUI::UDim(0.10f, 0), CEGUI::UDim(0.60f, 0), CEGUI::UDim(0.90f, 0)));
        m_pWindow->setAlpha(0.8f);

        // Give the window a caption
        m_pWindow->setText(CGUI_Impl::GetUTFString(szCaption));
    }

    m_pWindow->setDestroyedByParent(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    // Set fixed minimum size to 96x48
    //m_pWindow->setMetricsMode(CEGUI::Absolute);
    m_pWindow->setMinSize(CEGUI::USize(CEGUI::UDim(0, 96.0f), CEGUI::UDim(0, 48.0f)));

    // Don't enable the close button by default
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->setCloseButtonEnabled(false);

    // Some window specific style options
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->getTitlebar()->setFont("default-bold-small");

    // Register our events
    m_pWindow->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, &CGUIWindow_Impl::Event_OnCloseClick, this);
    AddEvents();

    // Disable rolling up, because we don't need it and it causes a freeze
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->setRollupEnabled(false);

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

CGUIWindow_Impl::~CGUIWindow_Impl()
{
    DestroyElement();
}

void CGUIWindow_Impl::SetMovable(bool bMovable)
{
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->setDragMovingEnabled(bMovable);
}

bool CGUIWindow_Impl::IsMovable()
{
    return reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->isDragMovingEnabled();
}

void CGUIWindow_Impl::SetSizingEnabled(bool bResizeEnabled)
{
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->setSizingEnabled(bResizeEnabled);
}

bool CGUIWindow_Impl::IsSizingEnabled()
{
    return reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->isSizingEnabled();
}

void CGUIWindow_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    m_pWindow->setProperty("FrameEnabled", bFrameEnabled ? "True" : "False");
}

bool CGUIWindow_Impl::IsFrameEnabled()
{
    return m_pWindow->getProperty("FrameEnabled") == "True" ? true : false;
}

void CGUIWindow_Impl::SetCloseButtonEnabled(bool bCloseButtonEnabled)
{
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->setCloseButtonEnabled(bCloseButtonEnabled);
}

bool CGUIWindow_Impl::IsCloseButtonEnabled()
{
    return reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->isCloseButtonEnabled();
}

void CGUIWindow_Impl::SetTitlebarEnabled(bool bTitlebarEnabled)
{
    reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->setTitleBarEnabled(bTitlebarEnabled);
}

bool CGUIWindow_Impl::IsTitlebarEnabled()
{
    return reinterpret_cast<CEGUI::FrameWindow*>(m_pWindow)->isTitleBarEnabled();
}

void CGUIWindow_Impl::SetCloseClickHandler(GUI_CALLBACK Callback)
{
    m_OnCloseClick = Callback;
}

bool CGUIWindow_Impl::Event_OnCloseClick(const CEGUI::EventArgs& e)
{
    if (m_OnCloseClick)
        m_OnCloseClick(reinterpret_cast<CGUIElement*>(this));
    return true;
}
