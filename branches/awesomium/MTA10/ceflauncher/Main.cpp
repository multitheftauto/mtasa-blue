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

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdShow, int nCmdShow )
{
    CefMainArgs mainArgs ( hInstance );
    CefRefPtr<CApp> app ( new CApp );

    return CefExecuteProcess ( mainArgs, app, NULL );
}
