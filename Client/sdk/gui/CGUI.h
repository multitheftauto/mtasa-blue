/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUIElement.h"
#include "CGUIWindow.h"

#include <../Shared/sdk/CRect2D.h>
#include <../Shared/sdk/CVector2D.h>

class CGUI
{
public:
    virtual void Draw() = 0;
    virtual void Draw(CGUIElement* element) = 0;

    virtual void Invalidate() = 0;
    virtual void Restore() = 0;

    virtual void SetHookedWindow(HWND window) = 0;
    virtual HWND GetHookedWindow() const = 0;

    virtual bool ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

    virtual CGUIWindow* CreateGUIWindow(CVector2D pos, CVector2D size, CGUIElement* parent, bool relative, std::string title) = 0;

    virtual void CreateDemo() = 0;
};
