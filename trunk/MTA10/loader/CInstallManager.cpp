/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    //////////////////////////////////////////////////////////
    //
    // Helper functions for this file
    //
    //
    //
    //////////////////////////////////////////////////////////

    // Ensure report log stuff has correct tags
    void UpdateSettingsForReportLog ( void )
    {
        UpdateMTAVersionApplicationSetting ();
        SetApplicationSetting ( "os-version",       GetOSVersion () );
        SetApplicationSetting ( "real-os-version",  GetRealOSVersion () );
        SetApplicationSetting ( "is-admin",         IsUserAdmin () ? "1" : "0" );
        SetApplicationSettingInt ( "last-server-ip", 0 );
    }

    // Comms between 'Admin' and 'User' processes
    void SendStringToUserProcess ( const SString& strText )
    {
        SetApplicationSetting ( "admin2user_comms", strText );
    }

    SString ReceiveStringFromAdminProcess ( void )
    {
        return GetApplicationSetting ( "admin2user_comms" );
    }

    bool IsBlockingUserProcess ( void )
    {
        return GetApplicationSetting ( "admin2user_comms" ) == "user_waiting";
    }

    void SetIsBlockingUserProcess ( void )
    {
        SetApplicationSetting ( "admin2user_comms", "user_waiting" );
    }

    void ClearIsBlockingUserProcess ( void )
    {
        if ( IsBlockingUserProcess () )
            SetApplicationSetting ( "admin2user_comms", "" );
    }
}


//////////////////////////////////////////////////////////
//
// CInstallManager global object
//
//
//
//////////////////////////////////////////////////////////
CInstallManager* g_pInstallManager = NULL;

CInstallManager* GetInstallManager ( void )
{
    if ( !g_pInstallManager )
        g_pInstallManager = new CInstallManager ();
    return g_pInstallManager;
}


//////////////////////////////////////////////////////////
//
// CInstallManager::InitSequencer
//
//
//
//////////////////////////////////////////////////////////
void CInstallManager::InitSequencer ( void )
{
    #define CR "\n"
    SString strSource =
                CR "initial: "                                      // *** Starts here  by default
                CR "            CALL CheckOnRestartCommand "
                CR "            IF LastResult != ok GOTO update_end: "
                CR " "                                              ////// Start of 'update game' //////
                CR "            CALL MaybeSwitchToTempExe "         // If update files comes with an .exe, switch to that for the install
                CR " "
                CR "copy_files: "
                CR "            CALL InstallFiles "                 // Try to install update files
                CR "            IF LastResult == ok GOTO update_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If install failed, try as admin
                CR " "
                CR "copy_files_admin: "
                CR "            CALL InstallFiles "                 // Try to install update files
                CR "            IF LastResult == ok GOTO update_end_admin: "
                CR " "
                CR "            CALL ShowCopyFailDialog "           // If install failed as admin, show message box
                CR "            IF LastResult == retry GOTO copy_files_admin: "
                CR " "
                CR "update_end_admin: "
                CR "            CALL ChangeFromAdmin "
                CR " "
                CR "update_end: "                                   ////// End of 'update game' //////
                CR "            CALL SwitchBackFromTempExe "
                CR " "        
                CR "newlayout_check:"                               ////// Start of 'new layout check' //////
                CR "            CALL ProcessLayoutChecks "
                CR "            IF LastResult == ok GOTO newlayout_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO newlayout_check: "
                CR "newlayout_end: "                                ////// End of 'new layout check' //////
                CR " "        
                CR "aero_check: "                                   ////// Start of 'Windows 7 aero desktop fix' //////
                CR "            CALL ProcessAeroChecks "            // Make changes to comply with user setting
                CR "            IF LastResult == ok GOTO aero_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO aero_check: "
                CR " "
                CR "aero_end: "                                     ////// End of 'Windows 7 aero desktop fix' //////
                CR " "        
                CR "largemem_check: "                               ////// Start of 'LargeMem fix' //////
                CR "            CALL ProcessLargeMemChecks "        // Make changes to comply with requirements
                CR "            IF LastResult == ok GOTO largemem_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO largemem_check: "
                CR " "
                CR "largemem_end: "                                 ////// End of 'LargeMem fix' //////
                CR " "        
                CR "dep_check: "                                    ////// Start of 'DEP fix' //////
                CR "            CALL ProcessDepChecks "             // Make changes to comply with user setting
                CR "            IF LastResult == ok GOTO dep_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO dep_check: "
                CR " "
                CR "dep_end: "                                      ////// End of 'DEP fix' //////
                CR " "        
                CR "nvidia_check: "                                 ////// Start of 'NVidia Optimus fix' //////
                CR "            CALL ProcessNvightmareChecks "      // Make changes to comply with requirements
                CR "            IF LastResult == ok GOTO nvidia_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO nvidia_check: "
                CR " "
                CR "nvidia_end: "                                   ////// End of 'NVidia Optimus fix' //////
                CR " "        
                CR "execopy_check: "                                ////// Start of 'Exe copy fix' //////
                CR "            CALL ProcessExeCopyChecks "         // Make changes to comply with requirements
                CR "            IF LastResult == ok GOTO execopy_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO execopy_check: "
                CR " "
                CR "execopy_end: "                                  ////// End of 'Exe copy fix' //////
                CR " "        
                CR "service_check: "                                ////// Start of 'Service checks' //////
                CR "            CALL ProcessServiceChecks "         // Make changes to comply with service requirements
                CR "            IF LastResult == ok GOTO service_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO service_check: "
                CR "            CALL Quit "
                CR " "
                CR "service_end: "                                  ////// End of 'Driver checks' //////
                CR "            CALL ChangeFromAdmin "              
                CR "            CALL InstallNewsItems "             // Install pending news
                CR "            GOTO launch: "
                CR " "
                CR "crashed: "                                      // *** Starts here when restarting after crash
                CR "            CALL ShowCrashFailDialog "
                CR "            IF LastResult == ok GOTO initial: "
                CR "            CALL Quit "
                CR " "
                CR "post_install: "                                 // *** Starts here when NSIS installer is nearly complete
                CR "            CALL HandlePostNsisInstall "
                CR "            CALL Quit "
                CR " "
                CR "pre_uninstall: "                                // *** Starts here when NSIS uninstaller is starting
                CR "            CALL HandlePreNsisUninstall "
                CR "            CALL Quit "
                CR " "
                CR "launch: ";


    m_pSequencer = new CSequencerType ();
    m_pSequencer->SetSource ( this, strSource );
    m_pSequencer->AddFunction ( "ShowCrashFailDialog",     &CInstallManager::_ShowCrashFailDialog );
    m_pSequencer->AddFunction ( "CheckOnRestartCommand",   &CInstallManager::_CheckOnRestartCommand );
    m_pSequencer->AddFunction ( "MaybeSwitchToTempExe",    &CInstallManager::_MaybeSwitchToTempExe );
    m_pSequencer->AddFunction ( "SwitchBackFromTempExe",   &CInstallManager::_SwitchBackFromTempExe );
    m_pSequencer->AddFunction ( "InstallFiles",            &CInstallManager::_InstallFiles );
    m_pSequencer->AddFunction ( "ChangeToAdmin",           &CInstallManager::_ChangeToAdmin );
    m_pSequencer->AddFunction ( "ShowCopyFailDialog",      &CInstallManager::_ShowCopyFailDialog );
    m_pSequencer->AddFunction ( "ProcessLayoutChecks",     &CInstallManager::_ProcessLayoutChecks );
    m_pSequencer->AddFunction ( "ProcessAeroChecks",       &CInstallManager::_ProcessAeroChecks );
    m_pSequencer->AddFunction ( "ProcessLargeMemChecks",   &CInstallManager::_ProcessLargeMemChecks );
    m_pSequencer->AddFunction ( "ProcessDepChecks",        &CInstallManager::_ProcessDepChecks );
    m_pSequencer->AddFunction ( "ProcessNvightmareChecks", &CInstallManager::_ProcessNvightmareChecks );
    m_pSequencer->AddFunction ( "ProcessExeCopyChecks",    &CInstallManager::_ProcessExeCopyChecks );
    m_pSequencer->AddFunction ( "ProcessServiceChecks",    &CInstallManager::_ProcessServiceChecks );
    m_pSequencer->AddFunction ( "ChangeFromAdmin",         &CInstallManager::_ChangeFromAdmin );
    m_pSequencer->AddFunction ( "InstallNewsItems",        &CInstallManager::_InstallNewsItems );
    m_pSequencer->AddFunction ( "Quit",                    &CInstallManager::_Quit );
    m_pSequencer->AddFunction ( "HandlePostNsisInstall",   &CInstallManager::_HandlePostNsisInstall );
    m_pSequencer->AddFunction ( "HandlePreNsisUninstall",  &CInstallManager::_HandlePreNsisUninstall );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::Continue
//
// Process next step
//
//////////////////////////////////////////////////////////
SString CInstallManager::Continue ( const SString& strCommandLineIn  )
{
    ShowSplash ( g_hInstance );

    // Update some settings which are used by ReportLog
    UpdateSettingsForReportLog ();
    ClearPendingBrowseToSolution ();

    // Restore sequencer
    InitSequencer ();
    RestoreSequencerFromSnapshot ( strCommandLineIn );

    // If command line says we're not running from the launch directory, get the launch directory location from the registry
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
        SetMTASAPathSource ( true );
    else
        SetMTASAPathSource ( false );

    // Initial report line
    DWORD dwProcessId = GetCurrentProcessId();
    SString GotPathFrom = ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" ) ? "registry" : "module location";
    AddReportLog ( 1041, SString ( "* Launch * pid:%d '%s' MTASAPath set from %s '%s'", dwProcessId, GetMTASAModuleFileName ().c_str (), GotPathFrom.c_str (), GetMTASAPath ().c_str () ) );

    // Run sequencer
    for ( int i = 0 ; !m_pSequencer->AtEnd () && i < 1000 ; i++ )
        m_pSequencer->ProcessNextLine ();

    // Remove unwanted files
    CleanDownloadCache ();

    // Extract command line launch args
    SString strCommandLineOut;
    for ( int i = 0 ; i < m_pSequencer->GetVariableInt ( "_argc" ) ; i++ )
        strCommandLineOut += m_pSequencer->GetVariable ( SString ( "_arg_%d", i ) ) + " ";

    AddReportLog ( 1060, SString ( "CInstallManager::Continue - return %s", *strCommandLineOut ) );
    return *strCommandLineOut.TrimEnd ( " " );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::RestoreSequencerFromSnapshot
//
// Set current sequencer position from a string
//
//////////////////////////////////////////////////////////
void CInstallManager::RestoreSequencerFromSnapshot ( const SString& strText )
{
    AddReportLog ( 1061, SString ( "CInstallManager::RestoreSequencerState %s", *strText ) );
    std::vector < SString > parts;
    strText.Split ( " ", parts );

    int iFirstArg = 0;
    if ( parts.size () > 0 && parts[0].Contains ( "=" ) )
    {
        // Upgrade variables
        if ( parts[0].Contains ( INSTALL_STAGE ) && !parts[0].Contains ( "_pc_label" ) )
            parts[0] = parts[0].Replace ( INSTALL_STAGE, "_pc_label" );

        m_pSequencer->RestoreStateFromString ( parts[0] );
        iFirstArg++;
    }

    // Add any extra command line args
    if ( m_pSequencer->GetVariableInt ( "_argc" ) == 0 )
    {
        m_pSequencer->SetVariable ( "_argc", parts.size () - iFirstArg );
        for ( uint i = iFirstArg ; i < parts.size () ; i++ )
        {
            m_pSequencer->SetVariable ( SString ( "_arg_%d", i - iFirstArg ), parts[i] );
        }
    }

    // Ignore LastResult
    m_pSequencer->SetVariable ( "LastResult", "ok" );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::GetSequencerSnapshot
//
// Save current sequencer position to a string
//
//////////////////////////////////////////////////////////
SString CInstallManager::GetSequencerSnapshot ( void )
{
    m_pSequencer->SetVariable ( "LastResult", "" );
    return m_pSequencer->SaveStateToString ();
}


//////////////////////////////////////////////////////////
//
// CInstallManager::GetLauncherPathFilename
//
// Get path to launch exe
//
//////////////////////////////////////////////////////////
SString CInstallManager::GetLauncherPathFilename ( void )
{
    SString strLocation = m_pSequencer->GetVariable ( INSTALL_LOCATION );
    SString strResult = PathJoin ( strLocation == "far" ? GetSystemCurrentDirectory () : GetMTASAPath (), MTA_EXE_NAME );
    AddReportLog ( 1062, SString ( "GetLauncherPathFilename %s", *strResult ) );
    return strResult;
}


//
// Functions called from the sequencer
//

//////////////////////////////////////////////////////////
//
// CInstallManager::_ChangeToAdmin
//
//
// Save the state of the sequencer and launch process as admin
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ChangeToAdmin ( void )
{
    if ( !IsUserAdmin () )
    {
        MessageBoxUTF8( NULL, SString ( _("MTA:SA needs Administrator access for the following task:\n\n  '%s'\n\nPlease confirm in the next window."), *m_strAdminReason ), "Multi Theft Auto: San Andreas", MB_OK | MB_TOPMOST  );
        SetIsBlockingUserProcess ();
        ReleaseSingleInstanceMutex ();
        if ( ShellExecuteBlocking ( "runas", GetLauncherPathFilename (), GetSequencerSnapshot () ) )
        {
            // Will return here once admin process has finished
            CreateSingleInstanceMutex ();
            UpdateSettingsForReportLog ();
            RestoreSequencerFromSnapshot ( ReceiveStringFromAdminProcess () );
            ClearIsBlockingUserProcess ();
            return "ok";    // This will appear as the result for _ChangeFromAdmin
        }
        CreateSingleInstanceMutex ();
        ClearIsBlockingUserProcess ();
        MessageBoxUTF8( NULL, SString ( _("MTA:SA could not complete the following task:\n\n  '%s'\n"), *m_strAdminReason ), "Multi Theft Auto: San Andreas"+_E("CL01"), MB_OK | MB_TOPMOST  );
    }
    return "fail";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ChangeFromAdmin
//
// Save the state of the sequencer and exit back to the user process
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ChangeFromAdmin ( void )
{
    if ( IsUserAdmin () && IsBlockingUserProcess () )
    {
        SendStringToUserProcess ( GetSequencerSnapshot () );
        AddReportLog ( 1003, SString ( "CInstallManager::_ChangeToAdmin - exit(0) %s", "" ) );
        ClearIsBlockingUserProcess ();
        ExitProcess ( 0 );
    }
    return "fail";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ShowCrashFailDialog
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ShowCrashFailDialog ( void )
{
    // Crashed before gta game started ?
    if ( WatchDogIsSectionOpen ( "L1" ) )
        WatchDogIncCounter ( "CR1" );

    HideSplash ();

    SString strMessage = GetApplicationSetting ( "diagnostics", "last-crash-info" );
    SString strReason = GetApplicationSetting ( "diagnostics", "last-crash-reason" );
    SetApplicationSetting ( "diagnostics", "last-crash-reason", "" );
    if ( strReason == "direct3ddevice-reset" )
    {
        strMessage += _("** The crash was caused by a graphics driver error **\n\n** Please update your graphics drivers **");
    }
    else
    {
        strMessage += strReason;
    }

    strMessage = strMessage.Replace ( "\r", "" ).Replace ( "\n", "\r\n" );
    SString strResult = ShowCrashedDialog ( g_hInstance, strMessage );
    HideCrashedDialog ();

    CheckAndShowFileOpenFailureMessage ();

    return strResult;
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_CheckOnRestartCommand
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_CheckOnRestartCommand ( void )
{
    // Check for pending update
    const SString strResult = CheckOnRestartCommand ();

    if ( strResult.Contains ( "install" ) )
    {
        // New settings for install
        m_pSequencer->SetVariable ( INSTALL_LOCATION, strResult.Contains ( "far" )    ? "far" : "near" );
        m_pSequencer->SetVariable ( SILENT_OPT,       strResult.Contains ( "silent" ) ? "yes" : "no" );
        return "ok";
    }
    else
    if ( !strResult.Contains ( "no update" ) )
    {
        AddReportLog ( 4047, SString ( "_CheckOnRestartCommand: CheckOnRestartCommand returned %s", strResult.c_str () ) );
    }

    return "no_action";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_MaybeSwitchToTempExe
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_MaybeSwitchToTempExe ( void )
{
    // If a new "Multi Theft Auto.exe" exists, let that complete the install
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
    {
        ReleaseSingleInstanceMutex ();
        if ( ShellExecuteNonBlocking ( "open", GetLauncherPathFilename (), GetSequencerSnapshot () ) )
            ExitProcess ( 0 );     // All done here
        CreateSingleInstanceMutex ();
        return "fail";
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_SwitchBackFromTempExe
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_SwitchBackFromTempExe ( void )
{
    // If currently running temp install exe, switch back
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
    {
        m_pSequencer->SetVariable ( INSTALL_LOCATION, "near" );

        ReleaseSingleInstanceMutex ();
        if ( ShellExecuteNonBlocking ( "open", GetLauncherPathFilename (), GetSequencerSnapshot () ) )
            ExitProcess ( 0 );     // All done here
        CreateSingleInstanceMutex ();
        return "fail";
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_InstallFiles
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_InstallFiles ( void )
{
    WatchDogReset ();

    // Install new files
    if ( !InstallFiles ( m_pSequencer->GetVariable ( SILENT_OPT ) != "no" ) )
    {
        if ( !IsUserAdmin () )
            AddReportLog ( 3048, SString ( "_InstallFiles: Install - trying as admin %s", "" ) );
        else
            AddReportLog ( 5049, SString ( "_InstallFiles: Couldn't install files %s", "" ) );

        m_strAdminReason = _("Install updated MTA:SA files");
        return "fail";
    }
    else
    {
        UpdateMTAVersionApplicationSetting ();
        AddReportLog ( 2050, SString ( "_InstallFiles: ok %s", "" ) );
        return "ok";
    }
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ShowCopyFailDialog
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ShowCopyFailDialog ( void )
{
    int iResponse = MessageBoxUTF8 ( NULL, _("Could not update due to file conflicts. Please close other applications and retry"), _("Error")+_E("CL02"), MB_RETRYCANCEL | MB_ICONERROR | MB_TOPMOST  );
    if ( iResponse == IDRETRY )
        return "retry";
    return "ok";
}


void ShowLayoutError ( const SString& strExtraInfo )
{
    MessageBoxUTF8 ( 0, SString ( _("Multi Theft Auto has not been installed properly, please reinstall. %s"), *strExtraInfo ), _("Error")+_E("CL03"), MB_OK | MB_TOPMOST  );
    TerminateProcess ( GetCurrentProcess (), 9 );
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessLayoutChecks
//
// Make sure new reg/dir structure is ok
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessLayoutChecks ( void )
{
    //
    // Validation
    //

    // Check data dir exists
    {
        if ( !DirectoryExists ( GetMTADataPath () ) )
            ShowLayoutError ( "[Data directory not present]" );   // Can't find directory
    }

    // Check reg key exists
    {
        if ( GetRegistryValue ( "", "Last Install Location" ).empty () )
            ShowLayoutError ( "[Registry key not present]" );   // Can't find reg key
    }

    // Check data dir writable
    {
        SString strTestFilePath = PathJoin ( GetMTADataPath (), "testdir", "testfile.txt" );

        FileDelete ( strTestFilePath );
        RemoveDirectory ( ExtractPath ( strTestFilePath ) );

        SString strContent = "test";
        if ( !FileSave ( strTestFilePath, strContent ) )
            ShowLayoutError ( "[Data directory not writable]" );   // Can't save file

        FileDelete ( strTestFilePath );
        RemoveDirectory ( ExtractPath ( strTestFilePath ) );
    }

    // Check reg key writable
    {
        RemoveRegistryKey ( "testkeypath" );

        SString strValue = GetTimeString( true, true );
        SetRegistryValue ( "testkeypath", "testname", strValue );
        SString strValueCheck = GetRegistryValue ( "testkeypath", "testname" );
        if ( strValueCheck != strValue )
            ShowLayoutError ( "[Registry key not writable]" );   // Can't write reg key

        RemoveRegistryKey ( "testkeypath" );
    }

    // Check install dir writable
    {
        SString strTestFilePath = CalcMTASAPath ( PathJoin ( "mta", "writetest.txt" ) );

        FileDelete ( strTestFilePath );

        SString strContent = "test";
        if ( !FileSave ( strTestFilePath, strContent ) )
            ShowLayoutError ( "[Install directory not writable]" );   // Can't save file

        FileDelete ( strTestFilePath );
    }

    //
    // Migration
    //

    // If news/temp/upcache folder doesn't exist in new, but does in old place, move it
    {
        const char* folders[] = { "news", "temp", "upcache" };
        for ( uint i = 0 ; i < NUMELMS( folders ) ; i++ )
        {
            SString strSrc = PathJoin ( GetSystemLocalAppDataPath (), "MTA San Andreas " + GetMajorVersionString (), folders[i] );
            SString strDest = PathJoin ( GetMTADataPath (), folders[i] );
            if ( !DirectoryExists ( strDest ) && DirectoryExists ( strSrc ) )
                MoveFile ( strSrc, strDest );
        }
    }

    // If aero option reg entry doesn't exist in new, but does in old place, move it
    {
        if ( GetApplicationSetting ( "aero-enabled" ).empty () )
        {
            SString strLegacyValue = GetVersionRegistryValueLegacy ( GetMajorVersionString (), PathJoin ( "Settings", "general" ), "aero-enabled" );
            if ( !strLegacyValue.empty () )
                SetApplicationSettingInt ( "aero-enabled", atoi ( strLegacyValue ) );
            else
                SetApplicationSettingInt ( "aero-enabled", 1 );
        }
    }

    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessAeroChecks
//
// Change the link timestamp in gta_sa.exe to trick windows 7 into using aero
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessAeroChecks ( void )
{
    SString strGTAPath;
    if ( GetGamePath ( strGTAPath ) == GAME_PATH_OK )
    {
        SString strGTAEXEPath = PathJoin ( strGTAPath , MTA_GTAEXE_NAME );
        // Get the top byte of the file link timestamp
        uchar ucTimeStamp = 0;
        FILE* fh = fopen ( strGTAEXEPath, "rb" );
        if ( fh )
        {
            if ( !fseek ( fh, 0x8B, SEEK_SET ) )
            {
                if ( fread ( &ucTimeStamp, sizeof ( ucTimeStamp ), 1, fh ) != 1 )
                {
                    ucTimeStamp = 0;
                }
            }
            fclose ( fh );
        }

        const uchar AERO_DISABLED = 0x42;
        const uchar AERO_ENABLED  = 0x43;

        // Check it's a value we're expecting
        bool bCanChangeAeroSetting = ( ucTimeStamp == AERO_DISABLED || ucTimeStamp == AERO_ENABLED );
        SetApplicationSettingInt ( "aero-changeable", bCanChangeAeroSetting );

        if ( bCanChangeAeroSetting )
        {
            // Get option to set
            bool bAeroEnabled = GetApplicationSettingInt ( "aero-enabled" ) ? true : false;
            uchar ucTimeStampRequired = bAeroEnabled ? AERO_ENABLED : AERO_DISABLED;
            if ( ucTimeStamp != ucTimeStampRequired )
            {
                // Change needed!
                SetFileAttributes ( strGTAEXEPath, FILE_ATTRIBUTE_NORMAL );
                FILE* fh = fopen ( strGTAEXEPath, "r+b" );
                if ( !fh )
                {
                    m_strAdminReason = _("Update Aero setting");
                    return "fail";
                }
                if ( !fseek ( fh, 0x8B, SEEK_SET ) )
                {
                    fwrite ( &ucTimeStampRequired, sizeof ( ucTimeStampRequired ), 1, fh );
                }
                fclose ( fh );
            }
        }
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessLargeMemChecks
//
// Change the PE header to give GTA access to more memory
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessLargeMemChecks ( void )
{
    SString strGTAPath;
    if ( GetGamePath ( strGTAPath ) == GAME_PATH_OK )
    {
        SString strGTAEXEPath = PathJoin ( strGTAPath , MTA_GTAEXE_NAME );
        // Get the top byte of the file link timestamp
        ushort usCharacteristics = 0;
        FILE* fh = fopen ( strGTAEXEPath, "rb" );
        if ( fh )
        {
            if ( !fseek ( fh, 0x96, SEEK_SET ) )
            {
                if ( fread ( &usCharacteristics, sizeof ( usCharacteristics ), 1, fh ) != 1 )
                {
                    usCharacteristics = 0;
                }
            }
            fclose ( fh );
        }

        const ushort LARGEMEM_DISABLED = 0x10f;
        const ushort LARGEMEM_ENABLED  = 0x12f;

        // Check it's a value we're expecting
        bool bCanChangeLargeMemSetting = ( usCharacteristics == LARGEMEM_DISABLED || usCharacteristics == LARGEMEM_ENABLED );

        if ( bCanChangeLargeMemSetting )
        {
            ushort usCharacteristicsRequired = LARGEMEM_ENABLED;
            if ( usCharacteristics != usCharacteristicsRequired )
            {
                // Change needed!
                SetFileAttributes ( strGTAEXEPath, FILE_ATTRIBUTE_NORMAL );
                FILE* fh = fopen ( strGTAEXEPath, "r+b" );
                if ( !fh )
                {
                    m_strAdminReason = _("Update Large Memory setting");
                    return "fail";
                }
                if ( !fseek ( fh, 0x96, SEEK_SET ) )
                {
                    fwrite ( &usCharacteristicsRequired, sizeof ( usCharacteristicsRequired ), 1, fh );
                }
                fclose ( fh );
            }
        }
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessDepChecks
//
// Change the PE header to enable DEP
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessDepChecks ( void )
{
    SString strGTAPath;
    if ( GetGamePath ( strGTAPath ) == GAME_PATH_OK )
    {
        SString strGTAEXEPath = PathJoin ( strGTAPath , MTA_GTAEXE_NAME );
        // Get the top byte of the file link timestamp
        ulong ulDllCharacteristics = 0;
        FILE* fh = fopen ( strGTAEXEPath, "rb" );
        if ( fh )
        {
            if ( !fseek ( fh, 0xDC, SEEK_SET ) )
            {
                if ( fread ( &ulDllCharacteristics, sizeof ( ulDllCharacteristics ), 1, fh ) != 1 )
                {
                    ulDllCharacteristics = 0;
                }
            }
            fclose ( fh );
        }

        const ulong DEP_DISABLED = 0x00000002;
        const ulong DEP_ENABLED  = 0x01000002;

        // Check it's a value we're expecting
        bool bCanChangeDepSetting = ( ulDllCharacteristics == DEP_DISABLED || ulDllCharacteristics == DEP_ENABLED );

        if ( bCanChangeDepSetting )
        {
            ulong ulDllCharacteristicsRequired = DEP_ENABLED;
            if ( ulDllCharacteristics != ulDllCharacteristicsRequired )
            {
                // Change needed!
                SetFileAttributes ( strGTAEXEPath, FILE_ATTRIBUTE_NORMAL );
                FILE* fh = fopen ( strGTAEXEPath, "r+b" );
                if ( !fh )
                {
                    m_strAdminReason = _("Update DEP setting");
                    return "fail";
                }
                if ( !fseek ( fh, 0xDC, SEEK_SET ) )
                {
                    fwrite ( &ulDllCharacteristicsRequired, sizeof ( ulDllCharacteristicsRequired ), 1, fh );
                }
                fclose ( fh );
            }
        }
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessNvightmareChecks
//
// Change the PE header to export 'NvOptimusEnablement'
//
//////////////////////////////////////////////////////////
namespace
{
    #define EU_VERSION_BYTE 0x004A1AA0     // Zero if US version 

    uint oldExportDir[] = { 0, 0 };
    uint newExportDir[] = { 0x004a32d0, 0x00000060 };
    uint oldExportTable[] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
    uint newExportTable[] = { 0x00000000, 0x51a9df70, 0x00000000, 0x004a3302,   //  ....pß©Q.....3J.
                              0x00000001, 0x00000001, 0x00000001, 0x004a32f8,   //  ............ø2J.
                              0x004a32fc, 0x004a3300, 0x004c6988, 0x004a3317,   //  ü/£..3J.ˆiL..3J.
                              0x74670000, 0x61735f61, 0x6578652e, 0x00000000,   //  ..gta_sa.exe....
                              0x00000000, 0x4e000000, 0x74704f76, 0x73756d69,   //  .......NvOptimus
                              0x62616e45, 0x656d656c, 0x0000746e, 0x00000000 }; //  Enablement......

    C_ASSERT( sizeof( oldExportDir ) == sizeof( newExportDir ) );
    C_ASSERT( sizeof( oldExportTable ) == sizeof( newExportTable ) );

    struct SDataumRow
    {
        uint uiFileOffsetUS;
        uint uiFileOffsetEU;
        void* pOldData;
        void* pNewData;
        uint uiDataSize;
    };

    // List of patchlets
    SDataumRow datumList[] = {
                    { 0x004A1AD0, 0x004A1ED0, oldExportTable,   newExportTable, sizeof( newExportTable ), },
                    { 0x000000F8, 0x000000F8, oldExportDir,     newExportDir,   sizeof( newExportDir ), },
                };

    SDataumRow valueItem = {
                    0x004C4588, 0x004C4988, NULL, NULL, 0
                };
}

SString CInstallManager::_ProcessNvightmareChecks ( void )
{
    SString strGTAPath;
    if ( GetGamePath( strGTAPath ) == GAME_PATH_OK )
    {
        SString strGTAEXEPath = PathJoin( strGTAPath , MTA_GTAEXE_NAME );

        char bIsEUVersion = false;
        bool bHasExportTable = true;
        uint uiExportValue = 0;
        bool bUnknownBytes = false;
        FILE* fh = fopen( strGTAEXEPath, "rb" );
        bool bFileError = ( fh == NULL );
        if( !bFileError )
        {
            // Determine version
            bFileError |= ( fseek( fh, EU_VERSION_BYTE, SEEK_SET ) != 0 );
            bFileError |= ( fread( &bIsEUVersion, 1, 1, fh ) != 1 );

            // Determine patched status
            for ( uint i = 0 ; i < NUMELMS( datumList ) ; i++ )
            {
                const SDataumRow& row = datumList[ i ];
                uint uiFileOffset = bIsEUVersion ? row.uiFileOffsetEU : row.uiFileOffsetUS;

                bFileError |= ( fseek( fh, uiFileOffset, SEEK_SET ) != 0 );

                std::vector < char > buffer( row.uiDataSize );
                bFileError |=  ( fread( &buffer[0], row.uiDataSize, 1, fh ) != 1 );

                if ( memcmp( &buffer[0], row.pOldData, row.uiDataSize ) == 0 )
                    bHasExportTable = false;
                else
                if ( memcmp( &buffer[0], row.pNewData, row.uiDataSize ) != 0 )
                    bUnknownBytes = true;
            }
            // Determine export value
            {
                uint uiFileOffset = bIsEUVersion ? valueItem.uiFileOffsetEU : valueItem.uiFileOffsetUS;
                bFileError |= ( fseek( fh, uiFileOffset, SEEK_SET ) != 0 );
                bFileError |= ( fread( &uiExportValue, sizeof( uiExportValue ), 1, fh ) != 1 );
            }
            fclose ( fh );
        }

        if ( bFileError )
            WriteDebugEvent( "Nvightmare patch: Can not apply due to unknown file error" );
        else
        if ( bUnknownBytes )
            WriteDebugEvent( "Nvightmare patch: Can not apply due to unknown file bytes" );
        else
        {
            // Determine if change required
            bool bReqExportTable = GetApplicationSettingInt( "nvhacks", "optimus-export-enablement" ) ? true : false;
            uint uiReqExportValue = 1;
            if ( bReqExportTable == bHasExportTable && uiReqExportValue == uiExportValue )
            {
                if ( bReqExportTable )
                    WriteDebugEvent( SString( "Nvightmare patch:  Already applied ExportValue of %d", uiReqExportValue ) );
            }
            else
            {
                // Change needed!
                SetFileAttributes( strGTAEXEPath, FILE_ATTRIBUTE_NORMAL );
                FILE* fh = fopen( strGTAEXEPath, "r+b" );
                if ( !fh )
                {
                    m_strAdminReason = _("Update graphics driver compliance");
                    return "fail";
                }

                bool bFileError = false;
                // Write patches
                if ( bReqExportTable != bHasExportTable )
                {
                    WriteDebugEvent( SString( "Nvightmare patch: Changing HasExportTable to %d", bReqExportTable ) );
                    for ( uint i = 0 ; i < NUMELMS( datumList ) ; i++ )
                    {
                        const SDataumRow& row = datumList[ i ];
                        uint uiFileOffset = bIsEUVersion ? row.uiFileOffsetEU : row.uiFileOffsetUS;

                        bFileError |= ( fseek( fh, uiFileOffset, SEEK_SET ) != 0 );
                        if ( bReqExportTable )
                            bFileError |= ( fwrite( row.pNewData, row.uiDataSize, 1, fh ) != 1 );
                        else
                            bFileError |= ( fwrite( row.pOldData, row.uiDataSize, 1, fh ) != 1 );
                    }
                }
                // Write value
                if ( uiReqExportValue != uiExportValue )
                {
                    WriteDebugEvent( SString( "Nvightmare patch: Changing ExportValue to %d", uiReqExportValue ) );
                    uint uiFileOffset = bIsEUVersion ? valueItem.uiFileOffsetEU : valueItem.uiFileOffsetUS;
                    bFileError |= ( fseek( fh, uiFileOffset, SEEK_SET ) != 0 );
                    bFileError |= ( fwrite( &uiReqExportValue, sizeof( uiReqExportValue ), 1, fh ) != 1 );
                }

                fclose ( fh );
                if ( bFileError )
                    WriteDebugEvent( "Nvightmare patch: File update completed with file errors" );
                else
                    WriteDebugEvent( "Nvightmare patch: File update completed" );
            }
        }
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::UpdateOptimusSymbolExport
//
// Try to update NvightmareChecks
// Return false if admin required
//
//////////////////////////////////////////////////////////
bool CInstallManager::UpdateOptimusSymbolExport( void )
{
    return _ProcessNvightmareChecks() == "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessExeCopyChecks
//
// Make copy of main exe if required
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessExeCopyChecks ( void )
{
    SString strGTAPath;
    if ( GetGamePath( strGTAPath ) == GAME_PATH_OK )
    {
        bool bCopyFailed;
        MaybeRenameExe( strGTAPath, &bCopyFailed );
        if ( bCopyFailed )
        {
            m_strAdminReason = _("Copy main executable to avoid graphic driver issues");
            return "fail";
        }
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessServiceChecks
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessServiceChecks ( void )
{
    if ( !CheckService ( CHECK_SERVICE_PRE_GAME ) )
    {
        if ( !IsUserAdmin() )
        {
            m_strAdminReason = _("Update install settings");
            return "fail";
        }
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_InstallNewsItems
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_InstallNewsItems ( void )
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
        SString strSavedDir = GetSystemCurrentDirectory ();

        // Calc and make target dir
        SString strTargetDir = PathJoin ( GetMTADataPath (), "news", strDate );
        MkDir ( strTargetDir );

        // Extract into target dir
        SetCurrentDirectory ( strTargetDir );

        // Try to extract the files
        if ( !ExtractFiles ( strFileLocation ) )
        {
            // If extract failed and update file is an exe, try to run it
            if ( ExtractExtension ( strFileLocation ).CompareI ( "exe" ) )
                ShellExecuteBlocking ( "open", strFileLocation, "-s" );
        }

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
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_Quit
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_Quit ( void )
{
    ExitProcess ( 0 );
    //return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_HandlePostNsisInstall
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_HandlePostNsisInstall ( void )
{
    if ( !CheckService ( CHECK_SERVICE_POST_INSTALL ) )
        return "fail";
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_HandlePreNsisUninstall
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_HandlePreNsisUninstall ( void )
{
    // stop & delete service
    if ( !CheckService ( CHECK_SERVICE_PRE_UNINSTALL ) )
        return "fail";
    return "ok";
}
