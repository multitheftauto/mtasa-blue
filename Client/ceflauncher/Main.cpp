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

int _declspec(dllimport) InitCEF ();

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdShow, int nCmdShow )
{
    return InitCEF();
}
