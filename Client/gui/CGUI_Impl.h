/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUI_Impl.h
 *  PURPOSE:     Graphical User Interface module class
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

private:

    IDirect3DDevice9* m_pDevice;
    HWND m_hookedWindow;

    bool m_bShowDemoWindow = true;
    bool m_bShowAnotherWindow = false;

    ImVec4 m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};
