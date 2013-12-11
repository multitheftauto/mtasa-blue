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
#include "SharedUtil.Win32Utf8FileHooks.hpp"
#if defined(_DEBUG) 
    #include "SharedUtil.Tests.hpp"
#endif


///////////////////////////////////////////////////////////////
//
// DoWinMain
//
// 'MTA San Andreas.exe' is launched as a subprocess under the following circumstances:
//      1. During install with /kdinstall command (as admin)
//      2. During uninstall with /kduninstall command (as admin)
//      3. By 'MTA San Andreas.exe' when temporary elevated privileges are required (as admin)
//      4. By 'MTA San Andreas.exe' during auto-update (in a temporary directory somewhere) (Which may then call it again as admin)
//
///////////////////////////////////////////////////////////////
extern "C" _declspec(dllexport)
int DoWinMain ( HINSTANCE hLauncherInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    AddUtf8FileHooks();

#if defined(_DEBUG) 
    SharedUtil_Tests ();
#endif

    //
    // Init
    //

    // Let install manager figure out what MTASA path to use
    GetInstallManager()->SetMTASAPathSource( lpCmdLine );

    // Start logging.....now
    BeginEventLog();

    // Start localization if possible
    InitLocalization( false );

    // Handle commands from the installer
    HandleSpecialLaunchOptions();

    // Check MTA is launched only once
    HandleDuplicateLaunching();

    // Show logo
    ShowSplash( g_hInstance );

    // Other init stuff
    ClearPendingBrowseToSolution();


    //
    // Update
    //

    // Continue any update procedure
    SString strCmdLine = GetInstallManager()->Continue();


    //
    // Launch
    //

    // Ensure localization is started
    InitLocalization( true );

    // Setup/test various counters and flags for monitoring problems
    PreLaunchWatchDogs();

    // Stuff
    HandleCustomStartMessage();
    CycleEventLog();
    BsodDetectionPreLaunch();
    MaybeShowCopySettingsDialog ();

    // Make sure GTA is not running
    HandleIfGTAIsAlreadyRunning();

    // Find GTA path to use
    ValidateGTAPath();

    // Maybe warn user if no anti-virus running
    CheckAntiVirusStatus();

    // Ensure logo is showing
    ShowSplash( g_hInstance );

    // Check MTA files look good
    CheckDataFiles();

    // Go for launch
    int iReturnCode = LaunchGame( strCmdLine );

    PostRunWatchDogs( iReturnCode );

    //
    // Quit
    //

    HandleOnQuitCommand();

    // Maybe show help if trouble was encountered
    ProcessPendingBrowseToSolution();

    AddReportLog ( 1044, SString ( "* End (%d)* pid:%d",  iReturnCode, GetCurrentProcessId() ) );

    RemoveUtf8FileHooks();
    return iReturnCode;
}
