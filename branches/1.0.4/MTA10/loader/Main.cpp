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
#include <time.h>

// Command line as map
class CUPL : public CArgMap
{
public:
    CUPL ( void )
        :  CArgMap ( "=", "&" )
    {}
};

// Common command line keys
#define INSTALL_STAGE       "install_stage"
#define INSTALL_LOCATION    "install_loc"
#define ADMIN_STATE         "admin_state"
#define SILENT_OPT          "silent_opt"


HINSTANCE g_hInstance = NULL;
LPSTR g_lpCmdLine = NULL;

class CMainHack
{
public:

///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    g_hInstance = hInstance;
    g_lpCmdLine = lpCmdLine;

#ifndef MTA_DEBUG
    ShowSplash ( hInstance );
#endif
    ClearPendingBrowseToSolution ();

    //////////////////////////////////////////////////////////
    //
    // Parse command line
    //

    CUPL UPL;
    UPL.SetFromString ( lpCmdLine );

    // Set command line defaults if required
    if ( !UPL.Contains ( INSTALL_STAGE ) )      UPL.Set ( INSTALL_STAGE, "initial" );
    if ( !UPL.Contains ( INSTALL_LOCATION ) )   UPL.Set ( INSTALL_LOCATION, "near" );
    if ( !UPL.Contains ( ADMIN_STATE ) )        UPL.Set ( ADMIN_STATE, "no" );   // Could be yes, but assuming 'no' works best

    // If not running from the launch directory, get the launch directory location from the registry
    if ( UPL.Get ( INSTALL_LOCATION ) == "far" )
        SetMTASAPathSource ( true );
    else
        SetMTASAPathSource ( false );

    // Update some settings which are used by ReportLog
    UpdateMTAVersionApplicationSetting ();
    SetApplicationSetting ( "os-version", GetOSVersion () );
    SetApplicationSetting ( "real-os-version", GetRealOSVersion () );
    SetApplicationSetting ( "is-admin", IsUserAdmin () ? "1" : "0" );

    // Initial report line
    DWORD dwProcessId = GetCurrentProcessId();
    SString GotPathFrom = ( UPL.Get( INSTALL_LOCATION ) == "far" ) ? "registry" : "module location";
    AddReportLog ( 1041, SString ( "* Launch * pid:%d '%s' MTASAPath set from %s '%s'", dwProcessId, GetMTASAModuleFileName ().c_str (), GotPathFrom.c_str (), GetMTASAPath ().c_str () ) );

    // Do next stage
    int iReturnCode = DoInstallStage ( UPL );

    AddReportLog ( 1044, SString ( "* End (%d)* pid:%d",  iReturnCode, dwProcessId ) );
    return iReturnCode;
}


//////////////////////////////////////////////////////////
//
// ChangeInstallUPL
//
//
//
//////////////////////////////////////////////////////////
int _ChangeInstallUPL ( const CUPL& UPLOld, const CUPL& UPLNew, bool bBlocking )
{
    AddReportLog ( 1045, SString ( "ChangeInstallUPL: '%s' -> '%s'", UPLOld.ToString ().c_str (), UPLNew.ToString ().c_str () ) );

    const SString strLocationOld    = UPLOld.Get ( INSTALL_LOCATION );
    const SString strAdminOld       = UPLOld.Get ( ADMIN_STATE );

    const SString strLocationNew    = UPLNew.Get ( INSTALL_LOCATION );
    const SString strAdminNew       = UPLNew.Get ( ADMIN_STATE );


    // If loc or admin state is changing, relaunch
    if ( strLocationOld != strLocationNew || strAdminOld != strAdminNew )
    {
        if ( strAdminOld == "yes" && strAdminNew == "no" )
            return 0;   // admin is always an inner process. Exit to drop rights.

        SString strAction = strAdminNew == "yes" ? "runas" : "open";
        SString strFile;
        if ( strLocationNew == "far" )
            strFile = PathJoin ( GetCurrentWorkingDirectory (), MTA_EXE_NAME_RELEASE );
        else
            strFile = PathJoin ( GetMTASAPath (), MTA_EXE_NAME_RELEASE );

        if ( bBlocking )
        {
            if ( ShellExecuteBlocking ( strAction, strFile, UPLNew.ToString () ) )
                return 0;
        }
        else
        {
            if ( ShellExecuteNonBlocking ( strAction, strFile, UPLNew.ToString () ) )
                return 0;
        }
    }

    return DoInstallStage ( UPLNew );
}


int ChangeInstallUPL ( const CUPL& UPLOld, const SString& strStageNew, const SString& strLocationNew, const SString& strAdminNew, const CUPL& UPLOptions = CUPL() )
{
    CUPL UPLNew = UPLOld;
    if ( strStageNew    != "*" )   UPLNew.Set ( INSTALL_STAGE, strStageNew );
    if ( strLocationNew != "*" )   UPLNew.Set ( INSTALL_LOCATION,   strLocationNew );
    if ( strAdminNew    != "*" )   UPLNew.Set ( ADMIN_STATE,   strAdminNew );
    UPLNew.Merge ( UPLOptions );
    return _ChangeInstallUPL ( UPLOld, UPLNew, false );
}

int ChangeInstallUPLBlocking ( const CUPL& UPLOld, const SString& strStageNew, const SString& strLocationNew, const SString& strAdminNew, const CUPL& UPLOptions = CUPL() )
{
    CUPL UPLNew = UPLOld;
    if ( strStageNew    != "*" )   UPLNew.Set ( INSTALL_STAGE, strStageNew );
    if ( strLocationNew != "*" )   UPLNew.Set ( INSTALL_LOCATION,   strLocationNew );
    if ( strAdminNew    != "*" )   UPLNew.Set ( ADMIN_STATE,   strAdminNew );
    UPLNew.Merge ( UPLOptions );
    return _ChangeInstallUPL ( UPLOld, UPLNew, true );
}


//////////////////////////////////////////////////////////
//
// DoInstallStage
//
//
//
//////////////////////////////////////////////////////////
int DoInstallStage ( const CUPL& UPL ) 
{
    AddReportLog ( 1046, SString ( "DoInstallStage: '%s'", UPL.ToString ().c_str () ) );

    if ( UPL.Get( INSTALL_STAGE ) == "crashed" )
    {
        // Crashed before gta game started ?
        if ( WatchDogIsSectionOpen ( "L1" ) )
            WatchDogIncCounter ( "CR1" );

        const SString strResult = HandlePostCrash ();
        if ( strResult.Contains ( "quit" ) )
            return 0;
        return ChangeInstallUPL ( UPL, "initial", "*", "*" );     
    }
    else
    if ( UPL.Get( INSTALL_STAGE ) == "initial" )
    {
        const SString strResult = CheckOnRestartCommand ();
        if ( strResult.Contains ( "install" ) )
        {
            SString strLocation = strResult.Contains ( "far" ) ? "far" : "near";
            CUPL UPLOptions;
            UPLOptions.Set ( SILENT_OPT, strResult.Contains ( "silent" ) ? "yes" : "no" );
            // This may launch new .exe in temp dir if required
            return ChangeInstallUPL ( UPL, "copy_files", strLocation, "no", UPLOptions );
        }
        else
        if ( !strResult.Contains ( "no update" ) )
        {
            AddReportLog ( 4047, SString ( "DoInstallStage: CheckOnRestartCommand returned %s", strResult.c_str () ) );
        }
    }
    else
    if ( UPL.Get( INSTALL_STAGE ) == "copy_files" )
    {
        WatchDogReset ();
        // Install new files
        if ( !InstallFiles ( UPL.Get( SILENT_OPT ) != "no" ) )
        {
            if ( UPL.Get( ADMIN_STATE ) != "yes" )
            {
                AddReportLog ( 3048, SString ( "DoInstallStage: Install - trying as admin %s", "" ) );
                // The only place the admin process is launched.
                // Wait for it to finish before continuing here.
                ChangeInstallUPLBlocking ( UPL, "*", "*", "yes" );
            }
            else
            {
                AddReportLog ( 5049, SString ( "DoInstallStage: Couldn't install files %s", "" ) );
                int iResponse = MessageBox ( NULL, "Could not update due to file conflicts. Please close other applications and retry", "Error", MB_RETRYCANCEL | MB_ICONERROR );
                if ( iResponse == IDRETRY )
                    return DoInstallStage ( UPL );
                //return 1;   // failure
            }
        }
        else
        {
            UpdateMTAVersionApplicationSetting ();
            AddReportLog ( 2050, SString ( "DoInstallStage: Install ok %s", "" ) );
        }
        return ChangeInstallUPL ( UPL, "launch", "near", "no" );
    }

    // Update news if any there
    InstallNewsItems ();

    // Default to launching MTA
    AddReportLog ( 1051, SString ( "DoInstallStage: LaunchGame cwd:%s", GetCurrentWorkingDirectory ().c_str () ) );
    return LaunchGame ( g_lpCmdLine );
}


//////////////////////////////////////////////////////////
//
// CheckOnRestartCommand
//
//
//
//////////////////////////////////////////////////////////
SString CheckOnRestartCommand ( void )
{
    const SString strMTASAPath = GetMTASAPath ();

    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

    SString strOperation, strFile, strParameters, strDirectory, strShowCmd;
    if ( GetOnRestartCommand ( strOperation, strFile, strParameters, strDirectory, strShowCmd ) )
    {
        if ( strOperation == "files" || strOperation == "silent" )
        {
            //
            // Update
            //

            // Make temp path name and go there
            SString strArchivePath, strArchiveName;
            strFile.Split ( "\\", &strArchivePath, &strArchiveName, -1 );

            SString strTempPath = MakeUniquePath ( strArchivePath + "\\_" + strArchiveName + "_tmp_" );

            if ( !MkDir ( strTempPath ) )
                return "FileError1";

            if ( !SetCurrentDirectory ( strTempPath ) )
                return "FileError2";

            // Start progress bar
            if ( strOperation != "silent" )
               StartPseudoProgress( g_hInstance, "MTA: San Andreas", "Extracting files..." );

            // Run self extracting archive to extract files into the temp directory
            ShellExecuteBlocking ( "open", strFile, "-s" );

            // Stop progress bar
            StopPseudoProgress();

            // If a new "Multi Theft Auto.exe" exists, let that complete the install
            if ( FileExists ( MTA_EXE_NAME_RELEASE ) )
                return "install from far " + strOperation;

            // Otherwise use the current exe to install
            return "install from near " + strOperation;
        }
        else
        {
            AddReportLog ( 5052, SString ( "CheckOnRestartCommand: Unknown restart command %s", strOperation.c_str () ) );
        }
    }
    return "no update";
}


//////////////////////////////////////////////////////////
//
// InstallNewsItems
//
//
//
//////////////////////////////////////////////////////////
void InstallNewsItems ( void )
{
    // Get install news queue
    CArgMap queue;
    queue.SetFromString ( GetApplicationSetting ( "news-install" ) );
    SetApplicationSetting ( "news-install", "" );

    std::vector < SString > keyList;
    queue.GetKeys ( keyList );
    for ( uint i = 0 ; i < keyList.size () ; i++ )
    {
        // Install each file
        SString strDate = keyList[i];
        SString strFileLocation = queue.Get ( strDate );

        // Save cwd
        SString strSavedDir = GetCurrentWorkingDirectory ();

        // Calc and make target dir
        SString strTargetDir = PathJoin ( GetMTALocalAppDataPath (), "news", strDate );
        MkDir ( strTargetDir );

        // Extract into target dir
        SetCurrentDirectory ( strTargetDir );
        ShellExecuteBlocking ( "open", strFileLocation, "-s" );

        // Restore cwd
        SetCurrentDirectory ( strSavedDir );

        // Check result
        if ( FileExists ( PathJoin ( strTargetDir, "files.xml" ) ) )
        {
            SetApplicationSettingInt ( "news-updated", 1 );
            AddReportLog ( 2051, SString ( "InstallNewsItems ok for '%s'", *strDate ) );
        }
        else
        {
            AddReportLog ( 4048, SString ( "InstallNewsItems failed with '%s' '%s' '%s'", *strDate, *strFileLocation, *strTargetDir ) );
        }
    }
}


//////////////////////////////////////////////////////////
//
// HandlePostCrash
//
//
//
//////////////////////////////////////////////////////////
SString HandlePostCrash ( void )
{
    HideSplash ();

    SString strMessage = GetApplicationSetting ( "diagnostics", "last-crash-info" );

    strMessage = strMessage.Replace ( "\r", "" ).Replace ( "\n", "\r\n" );

    SString strResult = ShowCrashedDialog ( g_hInstance, strMessage );
    HideCrashedDialog ();
    return strResult;
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
    int iResponse = MessageBox ( NULL, "There may be trouble.\n\nDo you want to revert to an earlier version of MTA:SA?", "", MB_YESNO | MB_ICONERROR );
    if ( iResponse == IDYES )
    {
        MessageBox ( NULL, "Your browser will now download an installer which may fix the trouble.\n\nIf the page fails to load, please goto www.mtasa.com", "", MB_OK | MB_ICONINFORMATION );
        BrowseToSolution ( "crashing-before-gtagame", false, true );
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
    // Check for unclean stop on previous run
    if ( WatchDogIsSectionOpen ( "L0" ) )
        WatchDogSetUncleanStop ( true );
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

    WatchDogBeginSection ( "L0" );
    WatchDogBeginSection ( "L1" );

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
    //////////////////////////////////////////////////////////
    //
    // Handle GTA already running
    //
    if ( !TerminateGTAIfRunning () )
    {
        DisplayErrorMessageBox ( "MTA: SA couldn't start because an another instance of GTA is running." );
        return 1;
    }

    //////////////////////////////////////////////////////////
    //
    // Get path to GTASA
    //
    SString strGTAPath;
    int iResult = GetGamePath ( strGTAPath );
    if ( iResult == 0 ) {
        DisplayErrorMessageBox ( "Registry entries are is missing. Please reinstall Multi Theft Auto: San Andreas.", "reg-entries-missing" );
        return 5;
    }
    else if ( iResult == -1 ) {
        DisplayErrorMessageBox ( "The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains only standard ASCII characters and reinstall Multi Theft Auto: San Andreas." );
        return 5;
    }
    else if ( iResult == -2 ) {
        DisplayErrorMessageBox ( "It appears you have a Steam version of GTA:SA, which is currently incompatible with MTASA.  You are now being redirected to a page where you can find information to resolve this issue." );
        BrowseToSolution ( "downgrade-steam" );
        return 5;
    }

    const SString strMTASAPath = GetMTASAPath ();

    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

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
        ShowSplash ( g_hInstance );
    #endif
    #endif

    //////////////////////////////////////////////////////////
    //
    // Basic check for some essential files
    //
    if ( !FileExists ( strMTASAPath + "\\mta\\cgui\\CGUI.png" ) )
    {
        // Check if CGUI.png exists
        return DisplayErrorMessageBox ( "Load failed. Please ensure that the data files have been installed correctly.", "mta-datafiles-missing" );
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

    SString strGTAEXEWindowedPath = strGTAPath + "\\" + MTA_GTAWINDOWEDEXE_NAME;
    if ( FileExists ( strGTAEXEWindowedPath ) && IsWindowedMode () && IsWin7OrHigher () )
        strGTAEXEPath = strGTAEXEWindowedPath;
   
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
                            "contact MTA at www.multitheftauto.com.", "createprocess-fail" );
        return 2;
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
        WaitForSingleObject ( piLoadee.hProcess, INFINITE );

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

};

// To avoid having to forward declare functions in this file
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    CMainHack MainHack;
    return MainHack.WinMain ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
}
