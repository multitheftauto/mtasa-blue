/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Main.cpp
*  PURPOSE:     MTA loader
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "Main.h"
#include "Utils.h"
#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>

#include "SharedUtil.h"
#include "SharedUtil.hpp"
using SharedUtil::CalcMTASAPath;

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce executable size
#define _WIN32_WINNT 0x0400     // So we can use IsDebuggerPresent()
#include <windows.h>
#endif

#include <shlobj.h>
#include <Psapi.h>

bool TerminateGTAIfRunning ( void )
{
    DWORD dwProcessIDs[250];
    DWORD pBytesReturned = 0;
    unsigned int uiListSize = 50;
    if ( EnumProcesses ( dwProcessIDs, 250 * sizeof(DWORD), &pBytesReturned ) )
    {
        for ( unsigned int i = 0; i < pBytesReturned / sizeof ( DWORD ); i++ )
        {
            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessIDs[i]);
            if ( hProcess )
            {
                HMODULE pModule;
                DWORD cbNeeded;
                if ( EnumProcessModules ( hProcess, &pModule, sizeof ( HMODULE ), &cbNeeded ) )
                {
                    char szModuleName[500];
                    if ( GetModuleFileNameEx( hProcess, pModule, szModuleName, 500 ) )
                    {
                        if ( strcmpi ( szModuleName + strlen ( szModuleName ) - strlen ( "gta_sa.exe" ), "gta_sa.exe" ) == 0 )
                        {
                            if ( MessageBox ( 0, "An instance of GTA: San Andreas is already running. It needs to be terminated before MTA: SA can be started. Do you want to do that now?", "Information", MB_YESNO | MB_ICONQUESTION ) == IDYES )
                            {
                                TerminateProcess ( hProcess, 0 );
                                CloseHandle ( hProcess );
                                return true;
                            }

                            return false;
                        } 
                    }
                }

                // Close the process
                CloseHandle ( hProcess );
            }
        }
    }

    return true;
}


//
// Returns true if the file exists
//
bool FileExists ( const SString& strFilename )
{
    WIN32_FIND_DATA fdFileInfo;
    HANDLE hHandle = FindFirstFile ( strFilename, &fdFileInfo );
    if ( hHandle == INVALID_HANDLE_VALUE )
        return false;
    FindClose ( hHandle );
    return true;
}

//
// General error message box
//
long DisplayErrorMessageBox ( HWND& hwndSplash, const SString& strMessage )
{
    if ( hwndSplash )
        DestroyWindow ( hwndSplash );
    MessageBox( 0, strMessage, "Error!", MB_ICONEXCLAMATION|MB_OK );
    return 1;
}


//
// Read a registry string value
//
bool ReadRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, char* szBuffer=NULL, DWORD dwBufferSize=0 )
{
    // Clear output
    if ( szBuffer && dwBufferSize )
        szBuffer[0] = 0;

    bool bResult = false;
    HKEY hkTemp = NULL;
    if ( RegOpenKeyEx ( hkRoot, szSubKey, 0, KEY_READ, &hkTemp ) == ERROR_SUCCESS ) 
    {
        if ( RegQueryValueEx ( hkTemp, szValue, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize ) == ERROR_SUCCESS )
        {
            bResult = true;
        }
        RegCloseKey ( hkTemp );
    }
    return bResult;
}

//
// Write a registry string value
//
void WriteRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, char* szBuffer )
{
    HKEY hkTemp;
    RegCreateKeyEx ( hkRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkTemp, NULL );
    if ( hkTemp )
    {
        RegSetValueEx ( hkTemp, szValue, NULL, REG_SZ, (LPBYTE)szBuffer, strlen(szBuffer) + 1 );
        RegCloseKey ( hkTemp );
    }
}


//
// UpgradeRegistryKeys
//
//                              Orig    New
//  "GTA:SA Path"               HKLM >> HKCU
//  "Last Run Location"         HKLM >> HKCU
//  "Last Install Location"     HKLM
//  "Serial"                    HKLM
//  "Username"                  HKLM
//
void UpgradeRegistryKeys ( void )
{
    char szSourceValue[MAX_PATH];
    bool bSourceExists;
    bool bDestExists;

    // Copy if source exists and destination does not

    // Upgrade "GTA:SA Path"
    bSourceExists = ReadRegistryStringValue ( HKEY_LOCAL_MACHINE, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path", szSourceValue, MAX_PATH - 1 ) && strlen ( szSourceValue );
    bDestExists   = ReadRegistryStringValue ( HKEY_CURRENT_USER,  "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path" );

    if ( bSourceExists && !bDestExists )
        WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path", szSourceValue );

    // Upgrade "Last Run Location"
    bSourceExists = ReadRegistryStringValue ( HKEY_LOCAL_MACHINE, "Software\\Multi Theft Auto: San Andreas", "Last Run Location", szSourceValue, MAX_PATH - 1 ) && strlen ( szSourceValue );
    bDestExists   = ReadRegistryStringValue ( HKEY_CURRENT_USER,  "Software\\Multi Theft Auto: San Andreas", "Last Run Location" );

    if ( bSourceExists && !bDestExists )
        WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "Last Run Location", szSourceValue );
}


void GetMTASAPath ( char * szBuffer, size_t sizeBufferSize )
{
    // Get current module full path
    GetModuleFileName ( NULL, szBuffer, sizeBufferSize - 1 );

    // Strip the module name out of the path.
    PathRemoveFileSpec ( szBuffer );

    // Save to a temp registry key
    WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "Last Run Location", szBuffer );
}


int GetGamePath ( char * szBuffer, size_t sizeBufferSize )
{
    char szRegBuffer[MAX_PATH];
    ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path", szRegBuffer, MAX_PATH - 1 );

    if ( ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) == 0 )
    {
        if ( strlen( szRegBuffer ) )
        {
			// Check for replacement characters (?), to see if there are any (unsupported) unicode characters
			if ( strchr ( szRegBuffer, '?' ) > 0 )
				return -1;

            char szExePath[MAX_PATH];
            sprintf ( szExePath, "%s\\%s", szRegBuffer, MTA_GTAEXE_NAME );
            if ( FileExists( szExePath  ) )
            {
                _snprintf ( szBuffer, sizeBufferSize, "%s", szRegBuffer );
                return 1;
            }
        }
    }
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Select your Grand Theft Auto: San Andreas Installation Directory";
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        // get the name of the  folder
        if ( !SHGetPathFromIDList ( pidl, szBuffer ) )
        {
            szBuffer = NULL;
        }

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
    
        if ( FileExists( SString ( "%s\\gta_sa.exe", szBuffer ) ) )
        {
            WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path", szBuffer );
        }
        else
        {
            if ( MessageBox ( NULL, "Could not find gta_sa.exe at the path you have selected. Choose another folder?", "Error", MB_OKCANCEL ) == IDOK )
            {
                return GetGamePath ( szBuffer, sizeBufferSize );
            }
            else
            {
                return 0;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{

    bool validate = false;

    if ( !TerminateGTAIfRunning () )
    {
        MessageBox ( 0, "MTA: SA couldn't start because an another instance of GTA is running.", "Error", MB_ICONERROR );
        return 0;
    }


    PROCESS_INFORMATION piLoadee;
    STARTUPINFO siLoadee;

    char szMTASAPath[MAX_PATH];
    char szGTAPath[MAX_PATH];

	int iResult;

    UpgradeRegistryKeys ();

	iResult = GetGamePath ( szGTAPath, MAX_PATH );
	if ( iResult == 0 ) {
		MessageBox ( 0, "Registry entries are is missing. Please reinstall Multi Theft Auto: San Andreas.", "Error!", MB_ICONEXCLAMATION | MB_OK );
        return 5;
	}
	else if ( iResult == -1 ) {
		MessageBox ( 0, "The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains only standard ASCII characters and reinstall Multi Theft Auto: San Andreas.", "Error!", MB_ICONEXCLAMATION | MB_OK );
		return 5;
	}

    GetMTASAPath ( szMTASAPath, MAX_PATH );

    // If we aren't compiling in debug-mode...
    HWND hwndSplash = NULL;
    #ifndef MTA_DEBUG
	#ifndef MTA_ALLOW_DEBUG
        // Are we debugged? Quit... if not compiled debug
        if ( IsDebuggerPresent () )
        {
            // Exit without a message so it'll be a little harder for the hacksors
            ExitProcess(-1);
        }

        // Show the splash and wait 2 seconds
        hwndSplash = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, 0 );
        Sleep ( 1000 );
	#endif
    #endif


    // Basic check for the data files
    if ( !FileExists ( SString ( "%s\\mta\\cgui\\CGUI.png", szMTASAPath ) ) )
    {
        // Check if CGUI.png exists
        return DisplayErrorMessageBox ( hwndSplash, "Load failed. Please ensure that the data files have been installed correctly." );
    }

    // Check for client file
    if ( !FileExists ( SString ( "%s\\%s", szMTASAPath, CHECK_DM_CLIENT_NAME ) ) )
    {
        return DisplayErrorMessageBox ( hwndSplash, "Load failed. Please ensure that '" CHECK_DM_CLIENT_NAME "' is installed correctly." );
    }

    // Check for lua file
    if ( !FileExists ( SString ( "%s\\%s", szMTASAPath, CHECK_DM_LUA_NAME ) ) )
    {
        return DisplayErrorMessageBox ( hwndSplash, "Load failed. Please ensure that '" CHECK_DM_LUA_NAME "' is installed correctly." );
    }

    // Grab the MTA folder
    char szDir[MAX_PATH];
    char szGTAEXEPath[MAX_PATH];
    strcpy ( szGTAEXEPath, szGTAPath );
    strcat ( szGTAEXEPath, "\\" );
    strcat ( szGTAEXEPath, MTA_GTAEXE_NAME ) ;
    strcpy ( szDir, szMTASAPath );
    strcat ( szDir, "\\mta" );
   
    // Make sure the gta executable exists
    SetCurrentDirectory ( szGTAPath );
    if ( !FileExists( szGTAEXEPath ) )
    {
        return DisplayErrorMessageBox ( hwndSplash, SString ( "Load failed. Could not find gta_sa.exe in %s.", szGTAPath ) );
    }

    // Make sure important dll's do not exist in the wrong place
    char* dllCheckList[] = { "xmll.dll", "cgui.dll", "net.dll", "libcurl.dll" };
    for ( int i = 0 ; i < NUMELMS ( dllCheckList ); i++ )
    {
        if ( FileExists( SString ( "%s\\%s", szGTAPath, dllCheckList[i] ) ) )
        {
            return DisplayErrorMessageBox ( hwndSplash, SString ( "Load failed. %s exists in the GTA directory. Please delete before continuing.", dllCheckList[i] ) );
        }    
    }

    // Launch GTA using CreateProcess
    memset( &piLoadee, 0, sizeof ( PROCESS_INFORMATION ) );
    memset( &siLoadee, 0, sizeof ( STARTUPINFO ) );
    siLoadee.cb = sizeof ( STARTUPINFO );

    // Start GTA
    if ( 0 == CreateProcess ( szGTAEXEPath,
                              lpCmdLine,
                              NULL,
                              NULL,
                              FALSE,
                              CREATE_SUSPENDED,
                              NULL,
                              szDir,
                              &siLoadee,
                              &piLoadee ) )
    {
        DisplayErrorMessageBox ( hwndSplash, "Could not start Grand Theft Auto: San Andreas.  "
                          "Please try restarting, or if the problem persists,"
                          "contact MTA at www.multitheftauto.com." );
        return 2;
    }

    SString strCoreDLL ( "%s\\mta\\%s", szMTASAPath, MTA_DLL_NAME );

    // Check if the core (mta_blue.dll or mta_blue_d.dll exists)
    if ( !FileExists ( strCoreDLL ) )
    {
        DisplayErrorMessageBox ( hwndSplash, "Load failed.  Please ensure that "
                          "the file core.dll is in the modules "
                           "directory within the MTA root directory." );

        // Kill GTA and return errorcode
        TerminateProcess ( piLoadee.hProcess, 1 );
        return 1;
    }

    // Check if the core can be loaded - failure may mean msvcr90.dll or d3dx9_40.dll etc is not installed
    HMODULE hCoreModule = LoadLibrary( strCoreDLL );
    if ( hCoreModule == NULL )
    {
        DisplayErrorMessageBox ( hwndSplash, "Load failed.  Please ensure that \n"
                            "Microsoft Visual C++ 2008 SP1 Redistributable Package (x86) \n"
                            "and the latest DirectX is correctly installed." );
        // Kill GTA and return errorcode
        TerminateProcess ( piLoadee.hProcess, 1 );
        return 1;
    }

    // Inject the core into GTA
    RemoteLoadLibrary ( piLoadee.hProcess, strCoreDLL );
    
    // Resume execution for the game.
    ResumeThread ( piLoadee.hThread );

    if ( hwndSplash )
        DestroyWindow ( hwndSplash );

	#ifdef MTA_DEBUG
	WaitForSingleObject ( piLoadee.hProcess, INFINITE );
	#endif

    // Cleanup and exit.
    CloseHandle ( piLoadee.hProcess );
    CloseHandle ( piLoadee.hThread );

    // Success
    return 0;
}
