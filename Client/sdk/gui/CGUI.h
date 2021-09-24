/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/gui/CGUI.h
 *  PURPOSE:     Graphical User Interface module interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CGUI;

#include <../Shared/sdk/CRect2D.h>

class CGUI
{
public:
    virtual void Draw() = 0;

    virtual void Invalidate() = 0;
    virtual void Restore() = 0;

    virtual void SetHookedWindow(HWND window) = 0;
    virtual HWND GetHookedWindow() const = 0;
};
