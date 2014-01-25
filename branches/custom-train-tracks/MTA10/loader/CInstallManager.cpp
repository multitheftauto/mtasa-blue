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
                CR "langfile_check: "                               ////// Start of 'Lang file fix' //////
                CR "            CALL ProcessLangFileChecks "        // Make changes to comply with requirements
                CR "            IF LastResult == ok GOTO langfile_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO langfile_check: "
                CR " "
                CR "langfile_end: "                                 ////// End of 'Lang file fix' //////
                CR " "        
                CR "exepatch_check: "                               ////// Start of 'Exe patch fix' //////
                CR "            CALL ProcessExePatchChecks "        // Make changes to comply with requirements
                CR "            IF LastResult == ok GOTO exepatch_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO exepatch_check: "
                CR " "
                CR "exepatch_end: "                                 ////// End of 'Exe patch fix' //////
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
    m_pSequencer->AddFunction ( "ProcessLangFileChecks",   &CInstallManager::_ProcessLangFileChecks );
    m_pSequencer->AddFunction ( "ProcessExePatchChecks",   &CInstallManager::_ProcessExePatchChecks );
    m_pSequencer->AddFunction ( "ProcessServiceChecks",    &CInstallManager::_ProcessServiceChecks );
    m_pSequencer->AddFunction ( "ChangeFromAdmin",         &CInstallManager::_ChangeFromAdmin );
    m_pSequencer->AddFunction ( "InstallNewsItems",        &CInstallManager::_InstallNewsItems );
    m_pSequencer->AddFunction ( "Quit",                    &CInstallManager::_Quit );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::SetMTASAPathSource
//
// Figure out and set the path that GetMTASAPath() should use
//
//////////////////////////////////////////////////////////
void CInstallManager::SetMTASAPathSource ( const SString& strCommandLineIn  )
{
    // Update some settings which are used by ReportLog
    UpdateSettingsForReportLog ();

    InitSequencer ();
    RestoreSequencerFromSnapshot ( strCommandLineIn );

    // If command line says we're not running from the launch directory, get the launch directory location from the registry
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
        ::SetMTASAPathSource ( true );
    else
        ::SetMTASAPathSource ( false );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::Continue
//
// Process next step
//
//////////////////////////////////////////////////////////
SString CInstallManager::Continue ( void )
{
    // Initial report line
    DWORD dwProcessId = GetCurrentProcessId();
    SString GotPathFrom = ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" ) ? "registry" : "module location";
    AddReportLog ( 1041, SString ( "* Launch * pid:%d '%s' MTASAPath set from %s '%s'", dwProcessId, GetLaunchPathFilename ().c_str (), GotPathFrom.c_str (), GetMTASAPath ().c_str () ) );

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

    // Transfer gta-fopen-last info
    if ( GetApplicationSetting ( "diagnostics", "gta-fopen-fail" ).empty() )
        SetApplicationSetting ( "diagnostics", "gta-fopen-fail", GetApplicationSetting ( "diagnostics", "gta-fopen-last" ) );
    SetApplicationSetting ( "diagnostics", "gta-fopen-last", "" );

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

    // Check data dirs exists and writable
    for ( uint i = 0 ; i < 2 ; i++ )
    {
        SString strMTADataPath = i ? GetMTADataPathCommon() : GetMTADataPath();

        if ( !DirectoryExists ( strMTADataPath ) )
            ShowLayoutError ( "[Data directory not present]" );   // Can't find directory

        SString strTestFilePath = PathJoin ( strMTADataPath, "testdir", "testfile.txt" );

        FileDelete ( strTestFilePath );
        RemoveDirectory ( ExtractPath ( strTestFilePath ) );

        SString strContent = "test";
        if ( !FileSave ( strTestFilePath, strContent ) )
            ShowLayoutError ( "[Data directory not writable]" );   // Can't save file

        FileDelete ( strTestFilePath );
        RemoveDirectory ( ExtractPath ( strTestFilePath ) );
    }

    // Check reg key exists
    {
        if ( GetRegistryValue ( "", "Last Install Location" ).empty () )
            ShowLayoutError ( "[Registry key not present]" );   // Can't find reg key
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

    //
    // Disk space check
    //
    SString strDriveWithNoSpace = GetDriveNameWithNotEnoughSpace();
    if ( !strDriveWithNoSpace.empty() )
    {
        SString strMessage( _("MTA:SA cannot continue because drive %s does not have enough space."), *strDriveWithNoSpace.Left( 1 ) );
        BrowseToSolution ( SString( "low-disk-space&drive=%s", *strDriveWithNoSpace ), ASK_GO_ONLINE | TERMINATE_PROCESS, strMessage );
    }

    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessLangFileChecks
//
// Make sure required language files exist
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessLangFileChecks( void )
{
    if ( !HasGTAPath() )
        return "ok";

    SString strTextFilePath = PathJoin( GetGTAPath(), "text" );
    const char* langFileNames[] = { "american.gxt", "french.gxt", "german.gxt", "italian.gxt", "spanish.gxt" };

    // Get language that will be used
    SString strSettingsFilename = PathJoin( GetSystemPersonalPath(), "GTA San Andreas User Files", "gta_sa.set" );
    FILE* fh = fopen( strSettingsFilename, "rb" );
    if ( fh )
    {
        fseek( fh, 0xB3B, SEEK_SET );
        char cLang = -1;
        fread( &cLang, 1, 1, fh );
        fclose( fh );
        if ( cLang >= 0 && cLang < NUMELMS( langFileNames ) )
        {
            // Check the selected language file is present
            SString strPathFilename = PathJoin( strTextFilePath, langFileNames[cLang] );
            if ( FileSize( strPathFilename ) > 10000 )
                return "ok";
        }
    }

    // Apply fix if the selected language file is not present, or there is no gta_sa.set file 

    // Find file to copy from
    SString strSrcFile;
    for( uint i = 0 ; i < NUMELMS( langFileNames ) ; i++ )
    {
        SString strPathFilename = PathJoin( strTextFilePath, langFileNames[i] );
        if ( FileSize( strPathFilename ) > 10000 )
        {
            strSrcFile = strPathFilename;
            break;
        }
    }

    if ( strSrcFile.empty() )
    {
        SString strMessage =  _("Missing file:");
        strMessage += "\n\n";
        strMessage += PathJoin( strTextFilePath, langFileNames[0] );
        strMessage += "\n\n";
        strMessage +=  _("If MTA fails to load, please re-install GTA:SA");
        MessageBoxUTF8( NULL, strMessage, "Multi Theft Auto: San Andreas", MB_OK | MB_TOPMOST  );
        return "ok";
    }

    // Fix missing files by copying
    for( uint i = 0 ; i < NUMELMS( langFileNames ) ; i++ )
    {
        SString strPathFilename = PathJoin( strTextFilePath, langFileNames[i] );
        if ( FileSize( strPathFilename ) < 10000 )
        {
            if ( !FileCopy( strSrcFile, strPathFilename ) )
            {
                m_strAdminReason = "Update language files";
                return "fail";
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
    return _ProcessExePatchChecks() == "ok";
}


//////////////////////////////////////////////////////////
//
// MaybeRenameExe
//
// Figure out whether to use a renamed exe, and return filename to use.
// Also tries file copy if required.
//
//////////////////////////////////////////////////////////
SString CInstallManager::MaybeRenameExe( const SString& strGTAPath )
{
    // Try patch/copy (may fail if not admin)
    _ProcessExePatchChecks();

    SString strGTAEXEPath = PathJoin( strGTAPath, MTA_GTAEXE_NAME );
    if ( ShouldUseExeCopy() )
    {
        // See if exe copy seems usable
        SString strHTAEXEPath = PathJoin( strGTAPath, MTA_HTAEXE_NAME );
        uint uiStdFileSize = FileSize( strGTAEXEPath );
        if ( uiStdFileSize && uiStdFileSize == FileSize( strHTAEXEPath ) )
            strGTAEXEPath = strHTAEXEPath;
    }

    return strGTAEXEPath;
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessExePatchChecks
//
// Check which changes need to be made to the exe and make a copy if required
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessExePatchChecks ( void )
{
    if ( !HasGTAPath() )
        return "ok";

    SExePatchedStatus reqStatus = GetExePatchRequirements();

    if ( !ShouldUseExeCopy() )
    {
        // Simple patching if won't be using a exe copy
        if ( !SetExePatchedStatus( false, reqStatus ) )
        {
            m_strAdminReason = GetPatchExeAdminReason( false, reqStatus );
            return "fail";
        }
    }
    else
    {
        // Bit more complercated if will be using a exe copy

        // First check if we need to create/update the exe copy
        if ( GetExePatchedStatus( true ) != reqStatus || GetExeFileSize( false ) != GetExeFileSize( true ) )
        {
            // Copy
            if ( !CopyExe() )
            {
                m_strAdminReason = GetPatchExeAdminReason( true, reqStatus );
                return "fail";
            }
            // Apply patches
            if ( !SetExePatchedStatus( true, reqStatus ) )
            {
                m_strAdminReason = GetPatchExeAdminReason( true, reqStatus );
                return "fail";
            }
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
