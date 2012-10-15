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

#include "StdInc.h"
#include "direct.h"
#if defined(_DEBUG) 
    #include "SharedUtil.Tests.hpp"
#endif
int DoLaunchGame ( LPSTR lpCmdLine );
int LaunchGame ( LPSTR lpCmdLine );

HINSTANCE g_hInstance = NULL;

///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain ( HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#if defined(_DEBUG) 
    SharedUtil_Tests ();
#endif

    //////////////////////////////////////////////////////////
    //
    // Handle duplicate launching, or running from mtasa:// URI ?
    //
    int iRecheckTimeLimit = 2000;
    while ( ! CreateSingleInstanceMutex () )
    {
        if ( strcmp ( lpCmdLine, "" ) != 0 )
        {
            COPYDATASTRUCT cdStruct;

            cdStruct.cbData = strlen(lpCmdLine)+1;
            cdStruct.lpData = const_cast<char *>((lpCmdLine));
            cdStruct.dwData = URI_CONNECT;

            HWND hwMTAWindow = FindWindow( NULL, "MTA: San Andreas" );
#ifdef MTA_DEBUG
            if( hwMTAWindow == NULL )
                hwMTAWindow = FindWindow( NULL, "MTA: San Andreas [DEBUG]" );
#endif
            if( hwMTAWindow != NULL )
            {
                SendMessage( hwMTAWindow,
                            WM_COPYDATA,
                            NULL,
                            (LPARAM)&cdStruct );
            }
            else
            {
                if ( iRecheckTimeLimit > 0 )
                {
                    // Sleep a little bit and check the mutex again
                    Sleep ( 500 );
                    iRecheckTimeLimit -= 500;
                    continue;
                }
                SString strMessage;
                strMessage += "Trouble restarting MTA:SA\n\n";
                strMessage += "If the problem persists, open Task Manager and\n";
                strMessage += "stop the 'gta_sa.exe' and 'Multi Theft Auto.exe' processes\n\n\n";
                strMessage += "Try to launch MTA:SA again?";
                if ( MessageBox( 0, strMessage, "Error", MB_ICONERROR | MB_YESNO ) == IDYES )
                {
                    TerminateGTAIfRunning ();
                    TerminateOtherMTAIfRunning ();
                    ShellExecuteNonBlocking( "open", PathJoin ( GetMTASAPath ( false ), MTA_EXE_NAME ), lpCmdLine );
                }
                return 0;
            }
        }
        else
        {
            if ( !IsGTARunning () && !IsOtherMTARunning () )
            {
                MessageBox ( 0, "Another instance of MTA is already running.\n\nIf this problem persists, please restart your computer", "Error", MB_ICONERROR );
            }
            else
            if ( MessageBox( 0, "Another instance of MTA is already running.\n\nDo you want to terminate it?", "Error", MB_ICONERROR | MB_YESNO ) == IDYES )
            {
                TerminateGTAIfRunning ();
                TerminateOtherMTAIfRunning ();
                ShellExecuteNonBlocking( "open", PathJoin ( GetMTASAPath ( false ), MTA_EXE_NAME ), lpCmdLine );
            }
        }
        return 0;
    }

    //
    // Continue any install procedure
    //
    SString strCmdLine = GetInstallManager ()->Continue ( lpCmdLine );

    //
    // Go for launch
    //
    int iReturnCode = LaunchGame ( const_cast < char* > ( *strCmdLine ) );

    AddReportLog ( 1044, SString ( "* End (%d)* pid:%d",  iReturnCode, GetCurrentProcessId() ) );
    return iReturnCode;
}


//////////////////////////////////////////////////////////
//
// HandleTrouble
//
//
//
//////////////////////////////////////////////////////////
void HandleTrouble ( void )
{
    if ( CheckAndShowFileOpenFailureMessage () )
        return;

    int iResponse = MessageBox ( NULL, "Are you having problems running MTA:SA?.\n\nDo you want to revert to an earlier version?", "MTA: San Andreas", MB_YESNO | MB_ICONERROR );
    if ( iResponse == IDYES )
    {
        MessageBox ( NULL, "Your browser will now display a web page with some help infomation.\n\nIf the page fails to load, please goto www.mtasa.com", "MTA: San Andreas", MB_OK | MB_ICONINFORMATION );
        BrowseToSolution ( "crashing-before-gtagame", false, true );
    }
}


//////////////////////////////////////////////////////////
//
// HandleResetSettings
//
//
//
//////////////////////////////////////////////////////////
void HandleResetSettings ( void )
{
    if ( CheckAndShowFileOpenFailureMessage () )
        return;

    char szResult[MAX_PATH] = "";
    SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, szResult );
    SString strSettingsFilename = PathJoin ( szResult, "GTA San Andreas User Files", "gta_sa.set" );
    SString strSettingsFilenameBak = PathJoin ( szResult, "GTA San Andreas User Files", "gta_sa_old.set" );

    if ( FileExists ( strSettingsFilename ) )
    {
        int iResponse = MessageBox ( NULL, "There seems to be a problem launching MTA:SA.\nResetting GTA settings can sometimes fix this problem.\n\nDo you want to reset GTA settings now?", "MTA: San Andreas", MB_YESNO | MB_ICONERROR );
        if ( iResponse == IDYES )
        {
            FileDelete ( strSettingsFilenameBak );
            FileRename ( strSettingsFilename, strSettingsFilenameBak );
            FileDelete ( strSettingsFilename );
            if ( !FileExists ( strSettingsFilename ) )
            {
                AddReportLog ( 4053, "Deleted gta_sa.set" );
                MessageBox ( NULL, "GTA settings have been reset.\n\nPress OK to continue.", "MTA: San Andreas", MB_OK | MB_ICONINFORMATION );
            }
            else
            {
                AddReportLog ( 5054, SString ( "Delete gta_sa.set failed with '%s'", *strSettingsFilename ) );
                MessageBox ( NULL, SString ( "File could not be deleted: '%s'", *strSettingsFilename ), "Error", MB_OK | MB_ICONERROR );
            }
        }
    }
    else
    {
        // No settings to delete, or can't find them
        int iResponse = MessageBox ( NULL, "Are you having problems running MTA:SA?.\n\nDo you want to see some online help?", "MTA: San Andreas", MB_YESNO | MB_ICONERROR );
        if ( iResponse == IDYES )
        {
            MessageBox ( NULL, "Your browser will now display a web page with some help infomation.\n\nIf the page fails to load, please goto www.mtasa.com", "MTA: San Andreas", MB_OK | MB_ICONINFORMATION );
            BrowseToSolution ( "crashing-before-gtalaunch", false, true );
        }
    }
}


//////////////////////////////////////////////////////////
//
// LaunchGame
//
//
//
//////////////////////////////////////////////////////////
int LaunchGame ( LPSTR lpCmdLine )
{
    //
    // "L0" is opened before the launch sequence and is closed if MTA shutsdown with no error
    // "L1" is opened before the launch sequence and is closed if GTA is succesfully started
    // "CR1" is a counter which is incremented if GTA was not started and MTA shutsdown with an error
    //
    // "L2" is opened before the launch sequence and is closed if the GTA loading screen is shown
    // "CR2" is a counter which is incremented at startup, if the previous run didn't make it to the loading screen
    //

    // Check for unclean stop on previous run
    if ( WatchDogIsSectionOpen ( "L0" ) )
        WatchDogSetUncleanStop ( true );    // Flag to maybe do things differently if MTA exit code on last run was not 0
    else
        WatchDogSetUncleanStop ( false );

    // Reset counter if gta game was run last time
    if ( !WatchDogIsSectionOpen ( "L1" ) )
        WatchDogClearCounter ( "CR1" );

    // If crashed 3 times in a row before starting the game, do something
    if ( WatchDogGetCounter ( "CR1" ) >= 3 )
    {
        WatchDogReset ();
        HandleTrouble ();
    }

    // Check for possible gta_sa.set problems
    if ( WatchDogIsSectionOpen ( "L2" ) )
    {
        WatchDogIncCounter ( "CR2" );       // Did not reach loading screen last time
        WatchDogCompletedSection ( "L2" );
    }
    else
        WatchDogClearCounter ( "CR2" );

    // If didn't reach loading screen 5 times in a row, do something
    if ( WatchDogGetCounter ( "CR2" ) >= 5 )
    {
        WatchDogClearCounter ( "CR2" );
        HandleResetSettings ();
    }

    WatchDogBeginSection ( "L0" );      // Gets closed if MTA exits with a return code of 0
    WatchDogBeginSection ( "L1" );      // Gets closed when online game has started
    SetApplicationSetting ( "diagnostics", "gta-fopen-fail", "" );
    SetApplicationSetting ( "diagnostics", "last-crash-reason", "" );

    int iReturnCode = DoLaunchGame ( lpCmdLine );

    if ( iReturnCode == 0 )
    {
        WatchDogClearCounter ( "CR1" );
        WatchDogCompletedSection ( "L0" );
    }

    return iReturnCode;
}


//////////////////////////////////////////////////////////
//
// DoLaunchGame
//
//
//
//////////////////////////////////////////////////////////
int DoLaunchGame ( LPSTR lpCmdLine )
{
    assert ( !CreateSingleInstanceMutex () );

    //////////////////////////////////////////////////////////
    //
    // Handle GTA already running
    //
    if ( IsGTARunning () )
    {
        if ( MessageBox ( 0, "An instance of GTA: San Andreas is already running. It needs to be terminated before MTA:SA can be started. Do you want to do that now?", "Information", MB_YESNO | MB_ICONQUESTION ) == IDYES )
        {
            TerminateGTAIfRunning ();
            if ( IsGTARunning () )
            {
                MessageBox ( 0, "Unable to terminate GTA: San Andreas. If the problem persists, please restart your computer.", "Information", MB_OK | MB_ICONQUESTION );
                return 1;
            }       
        }
        else
            return 0;
    }

    //////////////////////////////////////////////////////////
    //
    // Get path to GTASA
    //
    SString strGTAPath;
    ePathResult iResult = GetGamePath ( strGTAPath, true );
    if ( iResult == GAME_PATH_MISSING ) {
        DisplayErrorMessageBox ( "Registry entries are is missing. Please reinstall Multi Theft Auto: San Andreas.", "reg-entries-missing" );
        return 5;
    }
    else if ( iResult == GAME_PATH_UNICODE_CHARS ) {
        DisplayErrorMessageBox ( "The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains only standard ASCII characters and reinstall Multi Theft Auto: San Andreas." );
        return 5;
    }
    else if ( iResult == GAME_PATH_STEAM ) {
        DisplayErrorMessageBox ( "It appears you have a Steam version of GTA:SA, which is currently incompatible with MTASA.  You are now being redirected to a page where you can find information to resolve this issue." );
        BrowseToSolution ( "downgrade-steam" );
        return 5;
    }

    const SString strMTASAPath = GetMTASAPath ();

    if ( strGTAPath.Contains ( ";" ) || strMTASAPath.Contains ( ";" ) )
    {
        DisplayErrorMessageBox ( "The path to your installation of 'MTA:SA' or 'GTA: San Andreas'\n"
                                 "contains a ';' (semicolon).\n\n"
                                 " If you experience problems when running MTA:SA,\n"
                                 " move your installation(s) to a path that does not contain a semicolon." );
    }

    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

    //////////////////////////////////////////////////////////
    //
    // Show splash screen and wait 2 seconds
    //
    ShowSplash ( g_hInstance );

    //////////////////////////////////////////////////////////
    //
    // Basic check for some essential files
    //
    const char* dataFilesFiles [] = { "\\MTA\\cgui\\images\\background_logo.png"
                                     ,"\\MTA\\cgui\\images\\radarset\\up.png"
                                     ,"\\MTA\\D3DX9_42.dll"
                                     ,"\\MTA\\D3DCompiler_42.dll"
                                     ,"\\MTA\\bass.dll"
                                     ,"\\MTA\\bass_fx.dll"
                                     ,"\\MTA\\sa.dat"
                                     ,"\\MTA\\pthreadVC2.dll"
                                     ,"\\server\\pthreadVC2.dll"
                                     ,"\\server\\mods\\deathmatch\\libmysql.dll"};

    for ( uint i = 0 ; i < NUMELMS( dataFilesFiles ) ; i++ )
    {
        if ( !FileExists ( strMTASAPath + dataFilesFiles [ i ] ) )
        {
            return DisplayErrorMessageBox ( "Load failed. Please ensure that the latest data files have been installed correctly.", "mta-datafiles-missing" );
        }
    }

    if ( FileSize ( strMTASAPath + "\\MTA\\bass.dll" ) != 0x00018838 )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that the latest data files have been installed correctly.", "mta-datafiles-missing" );
    }

    // Check for client file
    if ( !FileExists ( strMTASAPath + "\\" +  CHECK_DM_CLIENT_NAME ) )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that '" CHECK_DM_CLIENT_NAME "' is installed correctly.", "client-missing" );
    }

    // Check for lua file
    if ( !FileExists ( strMTASAPath + "\\" + CHECK_DM_LUA_NAME ) )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that '" CHECK_DM_LUA_NAME "' is installed correctly.", "lua-missing" );
    }

    // Grab the MTA folder
    SString strGTAEXEPath = strGTAPath + "\\" + MTA_GTAEXE_NAME;
    SString strDir = strMTASAPath + "\\mta";

    // Make sure the gta executable exists
    SetCurrentDirectory ( strGTAPath );
    if ( !FileExists( strGTAEXEPath ) )
    {
        return DisplayErrorMessageBox ( SString ( "Load failed. Could not find gta_sa.exe in %s.", strGTAPath.c_str () ), "gta_sa-missing" );
    }

    // Make sure important dll's do not exist in the wrong place
    char* dllCheckList[] = { "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll" };
    for ( int i = 0 ; i < NUMELMS ( dllCheckList ); i++ )
    {
        if ( FileExists( strGTAPath + "\\" + dllCheckList[i] ) )
        {
            return DisplayErrorMessageBox ( SString ( "Load failed. %s exists in the GTA directory. Please delete before continuing.", dllCheckList[i] ), "file-clash" );
        }    
    }

    // Warning if d3d9.dll exists in the GTA install directory
    if ( FileExists( PathJoin ( strGTAPath, "d3d9.dll" ) ) )
    {
        ShowD3dDllDialog ( g_hInstance, PathJoin ( strGTAPath, "d3d9.dll" ) );
        HideD3dDllDialog ();
    }    

    // Strip out flag from command line
    SString strCmdLine = lpCmdLine;
    bool bDoneAdmin = strCmdLine.Contains ( "/done-admin" );
    strCmdLine = strCmdLine.Replace ( " /done-admin", "" );

    MaybeShowCopySettingsDialog ();
    SetDllDirectory( SString ( strMTASAPath + "\\mta" ) );

    //////////////////////////////////////////////////////////
    //
    // Hook 'n' go
    //
    // Launch GTA using CreateProcess
    PROCESS_INFORMATION piLoadee;
    STARTUPINFO siLoadee;
    memset( &piLoadee, 0, sizeof ( PROCESS_INFORMATION ) );
    memset( &siLoadee, 0, sizeof ( STARTUPINFO ) );
    siLoadee.cb = sizeof ( STARTUPINFO );

    WatchDogBeginSection ( "L2" );      // Gets closed when loading screen is shown

    // Start GTA
    if ( 0 == CreateProcess ( strGTAEXEPath,
                              (LPSTR)*strCmdLine,
                              NULL,
                              NULL,
                              FALSE,
                              CREATE_SUSPENDED,
                              NULL,
                              strDir,
                              &siLoadee,
                              &piLoadee ) )
    {
        DWORD dwError = GetLastError ();

        if ( dwError == ERROR_ELEVATION_REQUIRED && !bDoneAdmin )
        {
            // Try to relaunch as admin if not done so already
            ReleaseSingleInstanceMutex ();
            ShellExecuteNonBlocking( "runas", PathJoin ( strMTASAPath, MTA_EXE_NAME ), strCmdLine + " /done-admin" );            
            return 5;
        }
        else
        {
            // Otherwise, show error message
            SString strError = GetSystemErrorMessage ( dwError );            
            DisplayErrorMessageBox ( "Could not start Grand Theft Auto: San Andreas.  "
                                "Please try restarting, or if the problem persists,"
                                "contact MTA at www.multitheftauto.com. \n\n[" + strError + "]", "createprocess-fail;" + strError );
            return 5;
        }
    }

    SString strCoreDLL = strMTASAPath + "\\mta\\" + MTA_DLL_NAME;

    // Check if the core (mta_blue.dll or mta_blue_d.dll exists)
    if ( !FileExists ( strCoreDLL ) )
    {
        DisplayErrorMessageBox ( "Load failed.  Please ensure that "
                            "the file core.dll is in the modules "
                            "directory within the MTA root directory.", "core-missing" );

        // Kill GTA and return errorcode
        TerminateProcess ( piLoadee.hProcess, 1 );
        return 1;
    }

    SetDllDirectory( SString ( strMTASAPath + "\\mta" ) );

    // Check if the core can be loaded - failure may mean msvcr90.dll or d3dx9_40.dll etc is not installed
    HMODULE hCoreModule = LoadLibrary( strCoreDLL );
    if ( hCoreModule == NULL )
    {
        DisplayErrorMessageBox ( "Load failed.  Please ensure that \n"
                            "Microsoft Visual C++ 2008 SP1 Redistributable Package (x86) \n"
                            "and the latest DirectX is correctly installed.", "vc-redist-missing" );
        // Kill GTA and return errorcode
        TerminateProcess ( piLoadee.hProcess, 1 );
        return 1;
    }
    FreeLibrary ( hCoreModule );

    // Wait until the splash has been displayed the required amount of time
    HideSplash ( true );

    // Inject the core into GTA
    RemoteLoadLibrary ( piLoadee.hProcess, strCoreDLL );

    // Actually hide the splash
    HideSplash ();
    
    // Clear previous on quit commands
    SetOnQuitCommand ( "" );

    // Resume execution for the game.
    ResumeThread ( piLoadee.hThread );

    // Wait for game to exit
    if ( piLoadee.hThread)
        WaitForObject ( piLoadee.hProcess, NULL, INFINITE, g_hMutex );

    // Get its exit code
    DWORD dwExitCode = -1;
    GetExitCodeProcess( piLoadee.hProcess, &dwExitCode );

    //////////////////////////////////////////////////////////
    //
    // On exit
    //
    // Cleanup and exit.
    CloseHandle ( piLoadee.hProcess );
    CloseHandle ( piLoadee.hThread );
    ReleaseSingleInstanceMutex ();


    //////////////////////////////////////////////////////////
    //
    // Handle OnQuitCommand
    //
    // Maybe spawn an exe
    {
        SetCurrentDirectory ( strMTASAPath );
        SetDllDirectory( strMTASAPath );

        SString strOnQuitCommand = GetRegistryValue ( "", "OnQuitCommand" );

        std::vector < SString > vecParts;
        strOnQuitCommand.Split ( "\t", vecParts );
        if ( vecParts.size () > 4 && vecParts[0].length () )
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
                ShellExecuteNonBlocking( lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );            
            }
        }
    }

    // Maybe show help if trouble was encountered
    ProcessPendingBrowseToSolution ();

    // Success, maybe
    return dwExitCode;
}


extern "C" _declspec(dllexport)
int DoWinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    return WinMain ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
}

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    g_hInstance = hModule;
    return TRUE;
}
