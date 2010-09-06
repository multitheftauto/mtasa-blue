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

///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    //////////////////////////////////////////////////////////
    //
    // Handle relaunching to fix up permissions
    //
    if ( strcmp ( lpCmdLine, "fixpermissions" ) == 0 )
    {
        // Should be administrator here
        ShowProgressDialog ( hInstance, "Fixing MTA permissions...", true );
        FixPermissions ( GetMTASAPath () );
        HideProgressDialog ();
        return 0;
    }

    //////////////////////////////////////////////////////////
    //
    // Handle duplicate launching, or running from mtasa:// URI ?
    //
    //////////////////////////////////////////////////////////
    //
    // Handle GTA already running
    //
    if ( !TerminateGTAIfRunning () )
    {
        MessageBox ( 0, "MTA: SA couldn't start because an another instance of GTA is running.", "Error", MB_ICONERROR );
        return 0;
    }

    //////////////////////////////////////////////////////////
    //
    // Get path to GTASA
    //
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

    const SString strMTASAPath = GetMTASAPath ();

    //////////////////////////////////////////////////////////
    //
    // Show splash screen
    //
    // If we aren't compiling in debug-mode...
    #ifndef MTA_DEBUG
    #ifndef MTA_ALLOW_DEBUG
        // Are we debugged? Quit... if not compiled debug
        if ( IsDebuggerPresent () )
        {
            // Exit without a message so it'll be a little harder for the hacksors
            ExitProcess(-1);
        }

        // Show the splash and wait 2 seconds
        ShowSplash ( hInstance );
    #endif
    #endif


    //////////////////////////////////////////////////////////
    //
    // Check file and directory permissions are ok
    //
    if ( strcmp ( lpCmdLine, "skip_permissions_check" ) != 0 )
    {
        if ( !CheckPermissions ( strMTASAPath, 3000 ) )
        {
            HideSplash ();

            int hr = MessageBox ( 0, "MTA has detected that some of its files or directories do not have the correct permissions.\n\n"
                                 "Do you want to automatically fix this?", "Question", MB_YESNO | MB_ICONQUESTION );

            if ( hr == IDYES )
            {
                // Permissions are not correct, so elevate and fix them
                SetCurrentDirectory ( strMTASAPath );
                SetDllDirectory( strMTASAPath );
                SString strFile = strMTASAPath + "\\" + MTA_EXE_NAME;

                SHELLEXECUTEINFO info;
                memset( &info, 0, sizeof ( info ) );
                info.cbSize = sizeof ( info );
                info.fMask = SEE_MASK_NOCLOSEPROCESS;
                info.lpVerb = "runas";
                info.lpFile = strFile;
                info.lpParameters = "fixpermissions";
                info.lpDirectory = "";
                info.nShow = SW_SHOWNORMAL;
                ShellExecuteEx( &info );
                if ( info.hProcess)
                    WaitForSingleObject ( info.hProcess, INFINITE );
            }

            AddReportLog ( SString ( "WinMain: Fix permissions %s", ( hr == IDYES ) ? "yes" : "no" ) );
        }
    }


    //////////////////////////////////////////////////////////
    //
    // Handle OnRestartCommand
    //
    {
        SetCurrentDirectory ( strMTASAPath );
        SetDllDirectory( strMTASAPath );

        SString strOnRestartCommand = ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnRestartCommand" );
        SetOnRestartCommand ( "" );

        std::vector < SString > vecParts;
        strOnRestartCommand.Split ( "\t", vecParts );
        if ( vecParts.size () > 4 && vecParts[0].length () )
        {
            SString strOperation = vecParts[0];
            SString strFile = vecParts[1];
            SString strParameters = vecParts[2];
            SString strDirectory = vecParts[3];
            SString strShowCmd = vecParts[4];

            if ( strOperation == "files" || strOperation == "silent" )
            {
                bool bShowProgress = ( strOperation == "files" );
                if ( bShowProgress )
                {
                    ShowProgressDialog ( hInstance, "MTA: San Andreas" );
                    UpdateProgress( 10, 100, "Installing update..." );
                    Sleep ( 500 );
                    UpdateProgress( 30, 100 );
                }

                // Run self extracting archive
                SHELLEXECUTEINFO info;
                memset( &info, 0, sizeof ( info ) );
                info.cbSize = sizeof ( info );
                info.fMask = SEE_MASK_NOCLOSEPROCESS;
                info.lpVerb = "open";
                info.lpFile = strFile;
                info.lpParameters = "-s";
                info.lpDirectory = strMTASAPath;
                info.nShow = SW_SHOWNORMAL;
                ShellExecuteEx( &info );
                if ( info.hProcess)
                    WaitForSingleObject ( info.hProcess, INFINITE );

                if ( bShowProgress )
                {
                    UpdateProgress( 60, 100 );
                    Sleep ( 500 );
                    UpdateProgress( 90, 100 );
                    Sleep ( 100 );
                    HideProgressDialog ();
                }

                AddReportLog ( SString ( "WinMain: Update complete (%s) %s", strOperation.c_str (), strFile.c_str () ) );
            }
            else
            {
                AddReportLog ( SString ( "WinMain: Unknown restart command %s", strOperation.c_str () ) );
            }
        }
    }


    //////////////////////////////////////////////////////////
    //
    // Basic check for some essential files
    //
    if ( !FileExists ( strMTASAPath + "\\mta\\cgui\\CGUI.png" ) )
    {
        // Check if CGUI.png exists
        return DisplayErrorMessageBox ( "Load failed. Please ensure that the data files have been installed correctly." );
    }

    // Check for client file
    if ( !FileExists ( strMTASAPath + "\\" +  CHECK_DM_CLIENT_NAME ) )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that '" CHECK_DM_CLIENT_NAME "' is installed correctly." );
    }

    // Check for lua file
    if ( !FileExists ( strMTASAPath + "\\" + CHECK_DM_LUA_NAME ) )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that '" CHECK_DM_LUA_NAME "' is installed correctly." );
    }

    // Grab the MTA folder
    SString strGTAEXEPath = strGTAPath + "\\" + MTA_GTAEXE_NAME;
    SString strDir = strMTASAPath + "\\mta";
   
    // Make sure the gta executable exists
    SetCurrentDirectory ( strGTAPath );
    if ( !FileExists( strGTAEXEPath ) )
    {
        return DisplayErrorMessageBox ( SString ( "Load failed. Could not find gta_sa.exe in %s.", strGTAPath.c_str () ) );
    }

    // Make sure important dll's do not exist in the wrong place
    char* dllCheckList[] = { "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll" };
    for ( int i = 0 ; i < NUMELMS ( dllCheckList ); i++ )
    {
        if ( FileExists( strGTAPath + "\\" + dllCheckList[i] ) )
        {
            return DisplayErrorMessageBox ( SString ( "Load failed. %s exists in the GTA directory. Please delete before continuing.", dllCheckList[i] ) );
        }    
    }


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
        DisplayErrorMessageBox ( "Could not start Grand Theft Auto: San Andreas.  "
                            "Please try restarting, or if the problem persists,"
                            "contact MTA at www.multitheftauto.com." );
        return 2;
    }

    SString strCoreDLL = strMTASAPath + "\\mta\\" + MTA_DLL_NAME;

    // Check if the core (mta_blue.dll or mta_blue_d.dll exists)
    if ( !FileExists ( strCoreDLL ) )
    {
        DisplayErrorMessageBox ( "Load failed.  Please ensure that "
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
        DisplayErrorMessageBox ( "Load failed.  Please ensure that \n"
                            "Microsoft Visual C++ 2008 SP1 Redistributable Package (x86) \n"
                            "and the latest DirectX is correctly installed." );
        // Kill GTA and return errorcode
        TerminateProcess ( piLoadee.hProcess, 1 );
        return 1;
    }

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
        WaitForSingleObject ( piLoadee.hProcess, INFINITE );

    //////////////////////////////////////////////////////////
    //
    // On exit
    //
    // Cleanup and exit.
    CloseHandle ( piLoadee.hProcess );
    CloseHandle ( piLoadee.hThread );


    //////////////////////////////////////////////////////////
    //
    // Handle OnQuitCommand
    //
    // Maybe spawn an exe
    {
        SetCurrentDirectory ( strMTASAPath );
        SetDllDirectory( strMTASAPath );

        SString strOnQuitCommand = ReadRegistryStringValue ( HKEY_CURRENT_USER, "Software\\Multi Theft Auto: San Andreas", "OnQuitCommand" );

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
                ShellExecute( NULL, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );            
            }
        }
    }

    // Success
    return 0;
}

