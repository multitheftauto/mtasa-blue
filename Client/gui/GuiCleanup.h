/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/GuiCleanup.h
 *  PURPOSE:     Shared helpers for recursive CEGUI teardown
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <CEGUI.h>
#include <gui/CGUIElement.h>

inline void DestroyGuiWindowRecursive(CEGUI::Window* pWindow)
{
    if (!pWindow)
        return;

    const size_t childCount = pWindow->getChildCount();
    for (size_t idx = childCount; idx > 0; --idx)
    {
        if (CEGUI::Window* pChildWindow = pWindow->getChildAtIdx(static_cast<unsigned int>(idx - 1)))
            DestroyGuiWindowRecursive(pChildWindow);
    }

    if (void* pUserData = pWindow->getUserData())
    {
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>(pUserData);
        pWindow->setUserData(nullptr);
        delete pElement;
    }
}
