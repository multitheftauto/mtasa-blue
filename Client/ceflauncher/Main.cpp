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
#include <string>

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_cef_exe on the build server to generate new exe)
*/

int _declspec(dllimport) InitCEF();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdShow, int nCmdShow)
{
    return InitCEF();
}
