/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        utils/wpmhookdll/Main.cpp
*  PURPOSE:     Memory manipulation hooking utility
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <detours.h>
#include <cstdio>

using namespace std;

#define MESSAGE_MONITOR_START       0x8500
#define MESSAGE_MONITOR_END         0x8501
#define MESSAGE_MONITOR_RPM         0x8502
#define MESSAGE_MONITOR_WPM         0x8503

HWND hGTA = NULL;


// ******** ReadProcessMemory hook! *********
typedef BOOL ( WINAPI* pReadProcessMemory ) ( HANDLE hProcess,
                                              LPCVOID lpBaseAddress,
                                              LPVOID lpBuffer,
                                              SIZE_T nSize,
                                              SIZE_T* lpNumberOfBytesRead );

pReadProcessMemory pfnReadProcessMemory = NULL;

BOOL WINAPI Hook_ReadProcessMemory ( HANDLE hProcess,
                                     LPCVOID lpBaseAddress,
                                     LPVOID lpBuffer,
                                     SIZE_T nSize,
                                     SIZE_T* lpNumberOfBytesRead )
{
    // Tell MTA about the RPM call
    PostMessage ( hGTA, MESSAGE_MONITOR_RPM, reinterpret_cast < LPARAM > ( lpBaseAddress ), nSize );

    // Call the original WriteProcessMemory
    return pfnReadProcessMemory ( hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead );
}


// ******** WriteProcessMemory hook! *********
typedef BOOL ( WINAPI* pWriteProcessMemory ) ( HANDLE hProcess,
                                               LPVOID lpBaseAddress,
                                               LPCVOID lpBuffer,
                                               SIZE_T nSize,
                                               SIZE_T* lpNumberOfBytesWritten );

pWriteProcessMemory pfnWriteProcessMemory = NULL;

BOOL WINAPI Hook_WriteProcessMemory ( HANDLE hProcess,
                                      LPVOID lpBaseAddress,
                                      LPCVOID lpBuffer,
                                      SIZE_T nSize,
                                      SIZE_T* lpNumberOfBytesWritten )
{
    // Tell MTA about the WPM call
    PostMessage ( hGTA, MESSAGE_MONITOR_WPM, reinterpret_cast < LPARAM > ( lpBaseAddress ), nSize );

    // Call the original WriteProcessMemory
    return pfnWriteProcessMemory ( hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten );
}


void Initialize ( void )
{
    // Grab the GTA window handle
    hGTA = FindWindow ( "Grand theft auto San Andreas", NULL );
    if ( hGTA != NULL )
    {
        // Make sure Kernel32.dll is loaded
        PBYTE pKernel32 = reinterpret_cast < PBYTE > ( LoadLibrary ( "Kernel32.dll" ) );
        if ( pKernel32 )
        {
            // Hook ReadProcessMemory
	        pfnReadProcessMemory = reinterpret_cast < pReadProcessMemory > ( DetourFunction ( DetourFindFunction ( "Kernel32.dll", "ReadProcessMemory" ), 
                                                                                              reinterpret_cast < PBYTE > ( Hook_ReadProcessMemory ) ) );

            // Hook WriteProcessMemory
	        pfnWriteProcessMemory = reinterpret_cast < pWriteProcessMemory > ( DetourFunction ( DetourFindFunction ( "Kernel32.dll", "WriteProcessMemory" ), 
																						        reinterpret_cast < PBYTE > ( Hook_WriteProcessMemory ) ) );

            // Success?
            if ( pfnReadProcessMemory && pfnWriteProcessMemory )
            {
                // Tell MTA we successfully hooked the trainer
                PostMessage ( hGTA, MESSAGE_MONITOR_START, 0, 0 );
                return;
            }
        }

        // Tell MTA we successfully injected but failed hooking the functions
        PostMessage ( hGTA, MESSAGE_MONITOR_START, 1, 1 );
    }
}


void Finalize ( void )
{
    // Unhook WriteProcessMemory
    if ( pfnWriteProcessMemory )
    {
        DetourRemove ( reinterpret_cast < PBYTE > ( pfnWriteProcessMemory ), 
		       	       reinterpret_cast < PBYTE > ( Hook_WriteProcessMemory ) );
    }

    // Unhook ReadProcessMemory
    if ( pfnReadProcessMemory )
    {
        DetourRemove ( reinterpret_cast < PBYTE > ( pfnReadProcessMemory ), 
		       	       reinterpret_cast < PBYTE > ( Hook_ReadProcessMemory ) );
    }

    // Tell MTA we've unhooked from the trainer
    if ( hGTA != NULL )
    {
        PostMessage ( hGTA, MESSAGE_MONITOR_END, 0, 0 );
    }
}


int WINAPI DllMain ( HINSTANCE hModule, DWORD dwReason, PVOID pvNothing )
{
    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
        {
            Initialize ();
            return TRUE;
        }

        case DLL_PROCESS_DETACH:
        {
            Finalize ();
            return TRUE;
        }
    }

    return FALSE;
}
