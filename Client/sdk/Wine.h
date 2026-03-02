/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Wine/Proton detection utility
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <windows.h>

namespace Wine
{
    /////////////////////////////////////////////////////////
    //
    // IsRunningOnWine
    //
    // Detect if we are running under Wine/Proton
    //
    /////////////////////////////////////////////////////////
    inline bool IsRunningOnWine()
    {
        HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
        if (!hNtdll)
            return false;

        FARPROC wineVersion = GetProcAddress(hNtdll, "wine_get_version");
        return wineVersion != nullptr;
    }
}
