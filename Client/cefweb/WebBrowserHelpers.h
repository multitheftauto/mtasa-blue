/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/WebBrowserHelpers.h
 *  PURPOSE:     Webbrowser helper classes and functions
 *
 *****************************************************************************/
#pragma once
#include <windows.h>
#include <cef3/cef/include/cef_v8.h>
#include <SString.h>

bool isKeyDown(WPARAM wParam)
{
    return (GetKeyState(wParam) & 0x8000) != 0;
}

int GetCefKeyboardModifiers(WPARAM wParam, LPARAM lParam)
{
    // Optimization for Wine: Use GetKeyboardState to fetch all key states in one go
    // instead of multiple GetKeyState calls which can be slow due to round-trips.
    BYTE keyState[256];
    if (!GetKeyboardState(keyState))
        return 0;

    auto checkKey = [&](int key) { return (keyState[key] & 0x80) != 0; };
    auto checkToggle = [&](int key) { return (keyState[key] & 1) != 0; };

    int modifiers = 0;
    if (checkKey(VK_SHIFT))
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (checkKey(VK_CONTROL))
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (checkKey(VK_MENU))
        modifiers |= EVENTFLAG_ALT_DOWN;

    // Low bit set from GetKeyState indicates "toggled".
    if (checkToggle(VK_NUMLOCK))
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    if (checkToggle(VK_CAPITAL))
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;

    switch (wParam)
    {
        case VK_RETURN:
            if ((lParam >> 16) & KF_EXTENDED)
                modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_INSERT:
        case VK_DELETE:
        case VK_HOME:
        case VK_END:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_UP:
        case VK_DOWN:
        case VK_LEFT:
        case VK_RIGHT:
            if (!((lParam >> 16) & KF_EXTENDED))
                modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_NUMLOCK:
        case VK_NUMPAD0:
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
        case VK_DIVIDE:
        case VK_MULTIPLY:
        case VK_SUBTRACT:
        case VK_ADD:
        case VK_DECIMAL:
        case VK_CLEAR:
            modifiers |= EVENTFLAG_IS_KEY_PAD;
            break;
        case VK_SHIFT:
            if (checkKey(VK_LSHIFT))
                modifiers |= EVENTFLAG_IS_LEFT;
            else if (checkKey(VK_RSHIFT))
                modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        case VK_CONTROL:
            if (checkKey(VK_LCONTROL))
                modifiers |= EVENTFLAG_IS_LEFT;
            else if (checkKey(VK_RCONTROL))
                modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        case VK_MENU:
            if (checkKey(VK_LMENU))
                modifiers |= EVENTFLAG_IS_LEFT;
            else if (checkKey(VK_RMENU))
                modifiers |= EVENTFLAG_IS_RIGHT;
            break;
        case VK_LWIN:
            modifiers |= EVENTFLAG_IS_LEFT;
            break;
        case VK_RWIN:
            modifiers |= EVENTFLAG_IS_RIGHT;
            break;
    }
    return modifiers;
}
