/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIProgressBar_Impl.cpp
 *  PURPOSE:     Progress bar widget class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUILABEL_NAME "CGUI/ProgressBar"

CGUIProgressBar_Impl::CGUIProgressBar_Impl(CGUI_Impl* pGUI, CGUIElement* pParent)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUILABEL_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    AddEvents();

    reinterpret_cast<CEGUI::ProgressBar*>(m_pWindow)->setProgress(0);

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

CGUIProgressBar_Impl::~CGUIProgressBar_Impl()
{
    DestroyElement();
}

void CGUIProgressBar_Impl::SetProgress(float fProgress)
{
    reinterpret_cast<CEGUI::ProgressBar*>(m_pWindow)->setProgress(fProgress);
}

float CGUIProgressBar_Impl::GetProgress()
{
    return reinterpret_cast<CEGUI::ProgressBar*>(m_pWindow)->getProgress();
}
