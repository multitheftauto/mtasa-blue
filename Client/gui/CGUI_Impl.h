/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <gui/CGUI.h>
#include <list>
#include <windows.h>

class CGUI_Impl : public CGUI
{
public:
    CGUI_Impl(IDirect3DDevice9* pDevice);
    ~CGUI_Impl();

    void Draw();

    void Invalidate();
    void Restore();

    void SetHookedWindow(HWND window);
    HWND GetHookedWindow() const { return m_hookedWindow; }

    CGUIWindow* CreateGUIWindow(CVector2D pos, CVector2D size, CGUIElement* parent = nullptr, bool relative = false, std::string title = "MTA Window");

    bool ProcessWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void OnElementDestroy(CGUIElement* element);

    CVector2D GetResolution();

private:
    IDirect3DDevice9* m_pDevice;
    HWND              m_hookedWindow;

    std::list<CGUIElement*> m_guiElements;

    bool m_bShowDemoWindow = true;
    bool m_bShowAnotherWindow = false;

    ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};
