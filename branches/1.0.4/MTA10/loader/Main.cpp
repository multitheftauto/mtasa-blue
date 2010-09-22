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


// Command line as map
class CUPL : public CArgMap
{
public:
    CUPL ( void )
        :  CArgMap ( "=", "&" )
    {}
};

// Command line keys
#define INSTALL_STAGE  "install_stage"
#define INSTALL_LOC    "install_loc"
#define ADMIN_STATE    "admin_state"
#define SILENT_OPT     "silent_opt"

int     DoInstallStage              ( const CUPL& UPL );
SString CheckOnRestartCommand       ( void );
int     LaunchGame                  ( LPSTR lpCmdLine );

HINSTANCE g_hInstance = NULL;
LPSTR g_lpCmdLine = NULL;


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

    //////////////////////////////////////////////////////////
    //
    // Parse command line
    //

    CUPL UPL;
    UPL.SetFromString ( lpCmdLine );

    // No stage parameter means it's been run by the user
    if ( !UPL.Contains ( INSTALL_STAGE ) )
    {
        AddReportLog ( 1040, SString ( "*", 0 ) );
        UPL.Set ( INSTALL_STAGE, "initial" );
        UPL.Set ( INSTALL_LOC, "near" );
        UPL.Set ( ADMIN_STATE, "no" );   // Could be yes, but assuming 'no' works best
    }

    // If not running from the launch directory, get the launch directory location from the registry
    if ( UPL.Get ( INSTALL_LOC ) == "far" )
        SetMTASAPathSource ( true );
    else
        SetMTASAPathSource ( false );


    BOOL bIsAdmin = IsUserAdmin ();
    SString RealOSVersion = GetRealOSVersion ();

    DWORD pid = GetCurrentProcessId();
    SString GotPathFrom = ( UPL.Get( INSTALL_LOC ) == "far" ) ? "registry" : "module location";
    AddReportLog ( 1041, SString ( "* Launch * pid:%d '%s' RealOSVersion:%s IsUserAdmin:%d MTASAPath set from %s '%s'", pid, GetMTASAModuleFileName ().c_str (), RealOSVersion.c_str (), bIsAdmin, GotPathFrom.c_str (), GetMTASAPath ().c_str () ) );


    SString OSVersion = GetOSVersion ();
    if  ( OSVersion != RealOSVersion )
        AddReportLog ( 1043, SString ( "RealOSVersion:%s  OSVersion:%s", RealOSVersion.c_str (), OSVersion.c_str () ) );

    // Do next stage
    int iReturnCode = DoInstallStage ( UPL );

    AddReportLog ( 1044, SString ( "* End (%d)* pid:%d",  iReturnCode, pid ) );
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

    const SString strLocOld     = UPLOld.Get ( INSTALL_LOC );
    const SString strAdminOld   = UPLOld.Get ( ADMIN_STATE );

    const SString strLocNew     = UPLNew.Get ( INSTALL_LOC );
    const SString strAdminNew   = UPLNew.Get ( ADMIN_STATE );


    // If loc or admin state is changing, relaunch
    if ( strLocOld != strLocNew || strAdminOld != strAdminNew )
    {
        if ( strAdminOld == "yes" && strAdminNew == "no" )
            return 0;   // admin is always an inner process. Exit to drop rights.

        SString strAction = strAdminNew == "yes" ? "runas" : "open";
        SString strFile;
        if ( strLocNew == "far" )
            strFile = PathJoin ( GetCurrentWorkingDirectory (), "Multi Theft Auto.exe" );
        else
            strFile = PathJoin ( GetMTASAPath (), "Multi Theft Auto.exe" );

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


int ChangeInstallUPL ( const CUPL& UPLOld, const SString& strStageNew, const SString& strLocNew, const SString& strAdminNew, const CUPL& UPLOptions = CUPL() )
{
    CUPL UPLNew = UPLOld;
    if ( strStageNew != "*" )   UPLNew.Set ( INSTALL_STAGE, strStageNew );
    if ( strLocNew   != "*" )   UPLNew.Set ( INSTALL_LOC,   strLocNew );
    if ( strAdminNew != "*" )   UPLNew.Set ( ADMIN_STATE,   strAdminNew );
    UPLNew.Merge ( UPLOptions );
    return _ChangeInstallUPL ( UPLOld, UPLNew, false );
}

int ChangeInstallUPLBlocking ( const CUPL& UPLOld, const SString& strStageNew, const SString& strLocNew, const SString& strAdminNew, const CUPL& UPLOptions = CUPL() )
{
    CUPL UPLNew = UPLOld;
    if ( strStageNew != "*" )   UPLNew.Set ( INSTALL_STAGE, strStageNew );
    if ( strLocNew   != "*" )   UPLNew.Set ( INSTALL_LOC,   strLocNew );
    if ( strAdminNew != "*" )   UPLNew.Set ( ADMIN_STATE,   strAdminNew );
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

    if ( UPL.Get( INSTALL_STAGE ) == "initial" )
    {
        const SString strResult = CheckOnRestartCommand ();
        if ( strResult.Contains ( "install" ) )
        {
            SString strLoc = strResult.Contains ( "far" ) ? "far" : "near";
            CUPL UPLOptions;
            UPLOptions.Set ( SILENT_OPT, strResult.Contains ( "silent" ) ? "yes" : "no" );
            // This may launch new .exe in temp dir if required
            return ChangeInstallUPL ( UPL, "copy_files", strLoc, "no", UPLOptions );
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
                //return 1;   // failure
            }
        }
        else
            AddReportLog ( 2050, SString ( "DoInstallStage: Install ok %s", "" ) );

        return ChangeInstallUPL ( UPL, "launch", "near", "no" );
    }

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
            strFile.Split ( "\\", &strArchivePath, &strArchiveName, true );

            SString strTempPath = MakeUniquePath ( strArchivePath + "\\_" + strArchiveName + "_tmp_" );

            if ( !MkDir ( strTempPath ) )
                return "FileError1";

            if ( !SetCurrentDirectory ( strTempPath ) )
                return "FileError2";

            // Start progress bar
            if ( strOperation != "silent" )
               StartPseudoProgress( g_hInstance, "MTA: San Andreas", "Installing update..." );

            // Run self extracting archive to extract files into the temp directory
            ShellExecuteBlocking ( "open", strFile, "-s" );

            // Stop progress bar
            StopPseudoProgress();

            // If a new "Multi Theft Auto.exe" exists, let that complete the install
            if ( FileExists ( "Multi Theft Auto.exe" ) )
                return "install from far " + strOperation;

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
// LaunchGame
//
//
//
//////////////////////////////////////////////////////////
int LaunchGame ( LPSTR lpCmdLine )
{
    //////////////////////////////////////////////////////////
    //
    // Handle GTA already running
    //
    if ( !TerminateGTAIfRunning () )
    {
        DisplayErrorMessageBox ( "MTA: SA couldn't start because an another instance of GTA is running." );
        return 0;
    }

    //////////////////////////////////////////////////////////
    //
    // Get path to GTASA
    //
    SString strGTAPath;
    int iResult = GetGamePath ( strGTAPath );
    if ( iResult == 0 ) {
        DisplayErrorMessageBox ( "Registry entries are is missing. Please reinstall Multi Theft Auto: San Andreas." );
        return 5;
    }
    else if ( iResult == -1 ) {
        DisplayErrorMessageBox ( "The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains only standard ASCII characters and reinstall Multi Theft Auto: San Andreas." );
        return 5;
    }
    else if ( iResult == -2 ) {
        DisplayErrorMessageBox ( "It appears you have a Steam version of GTA:SA, which is currently incompatible with MTASA.  You are now being redirected to a page where you can find information to resolve this issue." );
        ShellExecute ( NULL, "open", "http://multitheftauto.com/downgrade/steam", NULL, NULL, SW_SHOWNORMAL );
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

