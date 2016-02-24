/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        ceflauncher/Main.cpp
*  PURPOSE:     CEF launcher entry point
*
*****************************************************************************/
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "CCefApp.h"
#include <string>

int _declspec(dllexport) InitCEF ()
{
    // Get absolute CEFLauncher.exe path
    TCHAR buffer[MAX_PATH];
    GetModuleFileName ( NULL, buffer, MAX_PATH );
    std::wstring currentFileName ( buffer );

    // Extract MTA path and set DLL directory (absolute path is required here)
    size_t pos = currentFileName.find_last_of ( L'\\' );
    std::wstring mtaPath = currentFileName.substr ( 0, pos - 3 ); // Strip "CEF"
    SetDllDirectory ( mtaPath.c_str () );

    // Load CEF
    CefMainArgs mainArgs ( GetModuleHandle(NULL) );
    CefRefPtr<CCefApp> app { new CCefApp };

    return CefExecuteProcess ( mainArgs, app, nullptr );
}
