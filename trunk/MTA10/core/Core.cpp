/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/Core.cpp
*  PURPOSE:     Core library entry point
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;

CCore* g_pCore = NULL;

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    CFilePathTranslator     FileTranslator;
    string                  WorkingDirectory;


    if ( dwReason == DLL_PROCESS_ATTACH )
    {
        FileTranslator.SetCurrentWorkingDirectory ( "mta" );
        FileTranslator.GetCurrentWorkingDirectory ( WorkingDirectory );
        SetCurrentDirectory ( WorkingDirectory.c_str ( ) );

        // For dll searches, this call replaces the current directory entry and turns off 'SafeDllSearchMode'
        // Meaning it will search the supplied path before the system and windows directory.
        // http://msdn.microsoft.com/en-us/library/ms682586%28VS.85%29.aspx
        SetDllDirectory( WorkingDirectory.c_str ( ) );

        g_pCore = new CCore;

        FileTranslator.GetGTARootDirectory ( WorkingDirectory );
        SetCurrentDirectory ( WorkingDirectory.c_str ( ) );
    } 
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        // For now, TerminateProcess if any destruction is attempted (or we'll crash)
        TerminateProcess ( GetCurrentProcess (), 0 );

        if ( g_pCore )
        {
            delete g_pCore;
            g_pCore = NULL;
        }
    }

    return TRUE;
}
