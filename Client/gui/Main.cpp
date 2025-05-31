/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/Main.cpp
 *  PURPOSE:     Graphical User Interface entry
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#define ALLOC_STATS_MODULE_NAME "cgui"
#include "SharedUtil.hpp"

CGUI_Impl* g_pGUI = NULL;

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    // Eventually destroy our GUI upon detach
    if (dwReason == DLL_PROCESS_DETACH && g_pGUI)
    {
        delete g_pGUI;
    }

    return TRUE;
}

MTAEXPORT CGUI* InitGUIInterface(IDirect3DDevice9* pDevice)
{
    // Create our GUI interface if not already done
    if (!g_pGUI)
    {
        g_pGUI = new CGUI_Impl(pDevice);
    }

    // Return it
    return g_pGUI;
}
