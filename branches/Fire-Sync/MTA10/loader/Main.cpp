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
#include "shellapi.h"

#include "SharedUtil.h"
#include "SharedUtil.hpp"
using SharedUtil::CalcMTASAPath;

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN     // Exclude all uncommon functions from windows.h to reduce executable size
//#define _WIN32_WINNT 0x0400     // So we can use IsDebuggerPresent()
#include <windows.h>
#endif

#include <shlobj.h>
#include <Psapi.h>

HANDLE g_hMutex = CreateMutex(NULL, FALSE, TEXT(MTA_GUID));

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

SString ReadRegistryStringValue ( HKEY hkRoot, LPCSTR szSubKey, LPCSTR szValue, int* iResult = NULL )
{
    // Clear output
    SString strOutResult = "";

    bool bResult = false;
    HKEY hkTemp = NULL;
    if ( RegOpenKeyEx ( hkRoot, szSubKey, 0, KEY_READ, &hkTemp ) == ERROR_SUCCESS ) 
    {
        DWORD dwBufferSize;
        if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, NULL, &dwBufferSize ) == ERROR_SUCCESS )
        {
            char *szBuffer = static_cast < char* > ( alloca ( dwBufferSize + 1 ) );
            if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize ) == ERROR_SUCCESS )
            {
                strOutResult = szBuffer;
                bResult = true;
            }
        }
        RegCloseKey ( hkTemp );
    }
    if ( iResult )
        *iResult = bResult;
    return strOutResult;
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


SString GetMTASAPath ( void )
{
    // Get current module full path
    char szBuffer[64000];
    GetModuleFileName ( NULL, szBuffer, sizeof(szBuffer) - 1 );

    // Strip the module name out of the path.
    PathRemoveFileSpec ( szBuffer );

    // Save to a temp registry key
    WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "Last Run Location", szBuffer );
    return szBuffer;
}


int GetGamePath ( SString& strOutResult )
{
    SString strRegPath = ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path" );

    if ( ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) == 0 )
    {
        if ( strlen( strRegPath.c_str () ) )
        {
            // Check for replacement characters (?), to see if there are any (unsupported) unicode characters
            if ( strchr ( strRegPath.c_str (), '?' ) > 0 )
                return -1;

            SString strExePath ( "%s\\%s", strRegPath.c_str (), MTA_GTAEXE_NAME );
            if ( FileExists( strExePath  ) )
            {
                strOutResult = strRegPath;
                return 1;
            }
            strExePath = SString( "%s\\%s", strRegPath.c_str (), MTA_GTASTEAMEXE_NAME );
            if ( FileExists( strExePath  ) )
            {
                return -2;
            }
        }
    }
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Select your Grand Theft Auto: San Andreas Installation Directory";
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 )
    {
        char szBuffer[MAX_PATH];
        // get the name of the  folder
        if ( SHGetPathFromIDListA ( pidl, szBuffer ) )
        {
            strOutResult = szBuffer;
        }

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
    
        if ( FileExists( SString ( "%s\\%s", strOutResult.c_str (), MTA_GTAEXE_NAME ) ) )
        {
            WriteRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "GTA:SA Path", strOutResult );
        }
        else
        {
            if ( MessageBox ( NULL, "Could not find gta_sa.exe at the path you have selected. Choose another folder?", "Error", MB_OKCANCEL ) == IDOK )
            {
                return GetGamePath ( strOutResult );
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

int CALLBACK DialogProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return 1;
}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        if ( strcmp ( lpCmdLine, "" ) != 0 )
        {
            COPYDATASTRUCT cdStruct;

            cdStruct.cbData = strlen(lpCmdLine)+1;
            cdStruct.lpData = const_cast<char *>((lpCmdLine));
            cdStruct.dwData = URI_CONNECT;

            HWND hwMTAWindow = FindWindow( NULL, "MTA: San Andreas" );
            if( hwMTAWindow != NULL )
            {
                SendMessage( hwMTAWindow,
                            WM_COPYDATA,
                            NULL,
                            (LPARAM)&cdStruct );
            }
            else
            {
                MessageBox( 0, "Can't send WM_COPYDATA", "Error", MB_ICONERROR );
                return 0;
            }
        }
        else
        {
            MessageBox ( 0, "Another instance of MTA is already running.", "Error", MB_ICONERROR );
            return 0;
        }
    }

    if ( !TerminateGTAIfRunning () )
    {
        MessageBox ( 0, "MTA: SA couldn't start because an another instance of GTA is running.", "Error", MB_ICONERROR );
        return 0;
    }

    PROCESS_INFORMATION piLoadee;
    STARTUPINFO siLoadee;

    SString strGTAPath;
    int iResult = GetGamePath ( strGTAPath );
    if ( iResult == 0 ) {
        MessageBox ( 0, "Registry entries are is missing. Please reinstall Multi Theft Auto: San Andreas.", "Error!", MB_ICONEXCLAMATION | MB_OK );
        return 5;
    }
    else if ( iResult == -1 ) {
        MessageBox ( 0, "The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains only standard ASCII characters and reinstall Multi Theft Auto: San Andreas.", "Error!", MB_ICONEXCLAMATION | MB_OK );
        return 5;
    }
    else if ( iResult == -2 ) {
        MessageBox ( 0, "It appears you have a Steam version of GTA:SA, which is currently incompatible with MTASA.  You are now being redirected to a page where you can find information to resolve this issue.", "Error", MB_OK|MB_ICONEXCLAMATION );
        ShellExecute ( NULL, "open", "http://multitheftauto.com/downgrade/steam", NULL, NULL, SW_SHOWNORMAL );
        return 5;
    }

    SString strMTASAPath = GetMTASAPath ();

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
        hwndSplash = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc );
        Sleep ( 1000 );
    #endif
    #endif


    // Basic check for the data files
    if ( !FileExists ( strMTASAPath + "\\mta\\cgui\\CGUI.png" ) )
    {
        // Check if CGUI.png exists
        return DisplayErrorMessageBox ( hwndSplash, "Load failed. Please ensure that the data files have been installed correctly." );
    }

    // Check for client file
    if ( !FileExists ( strMTASAPath + "\\" +  CHECK_DM_CLIENT_NAME ) )
    {
        return DisplayErrorMessageBox ( hwndSplash, "Load failed. Please ensure that '" CHECK_DM_CLIENT_NAME "' is installed correctly." );
    }

    // Check for lua file
    if ( !FileExists ( strMTASAPath + "\\" + CHECK_DM_LUA_NAME ) )
    {
        return DisplayErrorMessageBox ( hwndSplash, "Load failed. Please ensure that '" CHECK_DM_LUA_NAME "' is installed correctly." );
    }

    // Grab the MTA folder
    SString strGTAEXEPath = strGTAPath + "\\" + MTA_GTAEXE_NAME;
    SString strDir = strMTASAPath + "\\mta";
   
    // Make sure the gta executable exists
    SetCurrentDirectory ( strGTAPath );
    if ( !FileExists( strGTAEXEPath ) )
    {
        return DisplayErrorMessageBox ( hwndSplash, SString ( "Load failed. Could not find gta_sa.exe in %s.", strGTAPath.c_str () ) );
    }

    // Make sure important dll's do not exist in the wrong place
    char* dllCheckList[] = { "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll" };
    for ( int i = 0 ; i < NUMELMS ( dllCheckList ); i++ )
    {
        if ( FileExists( strGTAPath + "\\" + dllCheckList[i] ) )
        {
            return DisplayErrorMessageBox ( hwndSplash, SString ( "Load failed. %s exists in the GTA directory. Please delete before continuing.", dllCheckList[i] ) );
        }    
    }

    // Launch GTA using CreateProcess
    memset( &piLoadee, 0, sizeof ( PROCESS_INFORMATION ) );
    memset( &siLoadee, 0, sizeof ( STARTUPINFO ) );
    siLoadee.cb = sizeof ( STARTUPINFO );

    // Start GTA
    if ( 0 == CreateProcess ( strGTAEXEPath,
                              lpCmdLine,
                              NULL,
                              NULL,
                              FALSE,
                              CREATE_SUSPENDED,
                              NULL,
                              strDir,
                              &siLoadee,
                              &piLoadee ) )
    {
        DisplayErrorMessageBox ( hwndSplash, "Could not start Grand Theft Auto: San Andreas.  "
                          "Please try restarting, or if the problem persists,"
                          "contact MTA at www.multitheftauto.com." );
        return 2;
    }

    SString strCoreDLL = strMTASAPath + "\\mta\\" + MTA_DLL_NAME;

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

    SetDllDirectory( SString ( strMTASAPath + "\\mta" ) );

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
    
    // Clear previous on quit commands
    SetOnQuitCommand ( "" );

    // Resume execution for the game.
    ResumeThread ( piLoadee.hThread );

    if ( hwndSplash )
        DestroyWindow ( hwndSplash );

    //#ifdef MTA_DEBUG
    WaitForSingleObject ( piLoadee.hProcess, INFINITE );
    //#endif


    // Cleanup and exit.
    CloseHandle ( piLoadee.hProcess );
    CloseHandle ( piLoadee.hThread );
    CloseHandle ( g_hMutex );


    // Maybe spawn an exe
    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

    SString strOnQuitCommand = ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnQuitCommand" );

    std::vector < SString > vecParts;
    strOnQuitCommand.Split ( "\t", vecParts );
    if ( vecParts.size () > 4 )
    {
        SString strOperation = vecParts[0];
        SString strFile = vecParts[1];
        SString strParameters = vecParts[2];
        SString strDirectory = vecParts[3];
        SString strShowCmd = vecParts[4];

        if ( strOperation == "restart" )
        {
            strOperation = "open";
            strFile = strMTASAPath + "\\" + MTA_EXE_NAME;
        }

        LPCTSTR lpOperation     = strOperation == "" ? NULL : strOperation.c_str ();
        LPCTSTR lpFile          = strFile.c_str ();
        LPCTSTR lpParameters    = strParameters == "" ? NULL : strParameters.c_str ();
        LPCTSTR lpDirectory     = strDirectory == "" ? NULL : strDirectory.c_str ();
        INT nShowCmd            = strShowCmd == "" ? SW_SHOWNORMAL : atoi( strShowCmd );

        if ( lpOperation && lpFile )
        {
            ShellExecute( NULL, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );            
        }
    }

    // Success
    return 0;
}
