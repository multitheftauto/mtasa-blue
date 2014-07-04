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
bool IsRealDeal ( void );

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    CFilePathTranslator     FileTranslator;
    string                  WorkingDirectory;


    if ( dwReason == DLL_PROCESS_ATTACH )
    {
        if ( IsRealDeal () )
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
    } 
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        if ( IsRealDeal () )
        {
            // For now, TerminateProcess if any destruction is attempted (or we'll crash)
            TerminateProcess ( GetCurrentProcess (), 0 );

            if ( g_pCore )
            {
                delete g_pCore;
                g_pCore = NULL;
            }
        }
    }

    return TRUE;
}


//
// Returns true if dll has been loaded with GTA.
//
bool IsRealDeal ( void )
{
    static bool bResult = false;
    static bool bDone = false;
    if ( !bDone )
    {
        MEMORY_BASIC_INFORMATION info;
        VirtualQuery( (void*)0x0401000, &info, sizeof(MEMORY_BASIC_INFORMATION) );
        bResult = info.RegionSize > 0x0401000;
        bDone = true;
    }
    return bResult;
}
