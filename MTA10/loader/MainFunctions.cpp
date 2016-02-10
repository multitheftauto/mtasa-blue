/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/MainFunctions.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

DECLARE_ENUM( WSC_SECURITY_PROVIDER_HEALTH )
IMPLEMENT_ENUM_BEGIN( WSC_SECURITY_PROVIDER_HEALTH )
        ADD_ENUM( WSC_SECURITY_PROVIDER_HEALTH_GOOD,        "good" )
        ADD_ENUM( WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED,"not_monitored" )
        ADD_ENUM( WSC_SECURITY_PROVIDER_HEALTH_POOR,        "poor" )
        ADD_ENUM( WSC_SECURITY_PROVIDER_HEALTH_SNOOZE,      "snooze" )
IMPLEMENT_ENUM_END( "wsc_health" )

class CLocalizationDummy : public CLocalizationInterface
{
public:
    virtual SString             Translate                   ( const SString& strMessage ) { return strMessage; }
    virtual SString             TranslateWithContext        ( const SString& strContext, const SString& strMessage ) { return strMessage; }
    virtual SString             TranslatePlural             ( const SString& strSingular, const SString& strPlural, int iNum ) { return strPlural; }
    virtual SString             TranslatePluralWithContext  ( const SString& strContext, const SString& strSingular, const SString& strPlural, int iNum ) { return strPlural; }

    virtual std::map<SString,SString>   GetAvailableLanguages       ( void ) { return std::map<SString,SString>(); }
    virtual bool                        IsLocalized                 ( void ) { return false; }
    virtual SString                     GetLanguageDirectory        ( void ) { return ""; }
    virtual SString                     GetLanguageCode             ( void ) { return "en_US"; }
    virtual SString                     GetLanguageName             ( void ) { return "English"; }
};

CLocalizationInterface* g_pLocalization = new CLocalizationDummy();

//////////////////////////////////////////////////////////
//
// InitLocalization
//
// Start localization thingmy
//
//////////////////////////////////////////////////////////
void InitLocalization( bool bNoFail )
{
    static bool bDone = false;
    if ( bDone )
        return;

    // Check for and load core.dll for localization
    // Use launch relative path so core.dll can get updated
    SString strCoreDLL = PathJoin( GetLaunchPath(), "mta", MTA_DLL_NAME );
    if ( !FileExists ( strCoreDLL ) )
    {
        if ( !bNoFail )
            return;
        DisplayErrorMessageBox ( ("Load failed.  Please ensure that "
                            "the file core.dll is in the modules "
                            "directory within the MTA root directory."), _E("CL23"), "core-missing" ); // Core.dll missing

        return ExitProcess( EXIT_ERROR );
    }

    // Use registry setting of mta path for dlls, as they will not be present in update files
    const SString strMTASAPath = GetMTASAPath ();
    SetDllDirectory( PathJoin( strMTASAPath, "mta" ) );

    // See if xinput is loadable (XInput9_1_0.dll is core.dll dependency)
    HMODULE hXInputModule = LoadLibrary( "XInput9_1_0.dll" );
    if ( hXInputModule )
        FreeLibrary( hXInputModule );
    else
    {
        // If not, do hack to use dll supplied with MTA
        SString strDest = PathJoin( strMTASAPath, "mta", "XInput9_1_0.dll" );
        if ( !FileExists( strDest ) )
        {
            SString strSrc = PathJoin( strMTASAPath, "mta", "XInput9_1_0_mta.dll" );       
            FileCopy( strSrc, strDest );
        }
    }

    // Check if the core can be loaded - failure may mean msvcr90.dll or d3dx9_40.dll etc is not installed
    // Use LOAD_WITH_ALTERED_SEARCH_PATH so the strCoreDLL path is searched first for dependent dlls
    HMODULE hCoreModule = LoadLibraryEx( strCoreDLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
    if ( hCoreModule == NULL )
    {
        if ( !bNoFail )
            return;
        DisplayErrorMessageBox ( ("Loading core failed.  Please ensure that \n"
                            "Microsoft Visual C++ 2013 Redistributable Package (x86) \n"
                            "and the latest DirectX is correctly installed."), _E("CL24"), "vc-redist-missing" );  // Core.dll load failed.  Ensure VC++ Redists and DX are installed
        return ExitProcess( EXIT_ERROR );
    }

    // Grab our locale from the registry if possible, if not Windows
    SString strLocale = GetApplicationSetting ( "locale" );
    if ( strLocale.empty() )
    {
        setlocale(LC_ALL, "");
        char* szLocale = setlocale(LC_ALL, NULL);
        strLocale = szLocale;
    }

    typedef CLocalizationInterface* (__cdecl *FUNC_CREATELOCALIZATIONFROMENVIRONMENT)(SString strLocale);
    FUNC_CREATELOCALIZATIONFROMENVIRONMENT pFunc = (FUNC_CREATELOCALIZATIONFROMENVIRONMENT)GetProcAddress ( hCoreModule, "L10n_CreateLocalization" );
    CLocalizationInterface* pLocalization = pFunc(strLocale);
    if ( pLocalization == NULL )
    {
        if ( !bNoFail )
            return;

        DisplayErrorMessageBox ( ("Loading core failed.  Please ensure that \n"
                            "Microsoft Visual C++ 2013 Redistributable Package (x86) \n"
                            "and the latest DirectX is correctly installed."), _E("CL26"), "vc-redist-missing" );  // Core.dll load failed.  Ensure VC++ Redists and DX are installed
        FreeLibrary ( hCoreModule );
        return ExitProcess( EXIT_ERROR );
    }

    SAFE_DELETE( g_pLocalization );
    g_pLocalization = pLocalization;
    bDone = true;

#ifdef MTA_DEBUG
    TestDialogs();
#endif
}


//////////////////////////////////////////////////////////
//
// HandleSpecialLaunchOptions
//
// Check and handle commands (from the installer)
//
//////////////////////////////////////////////////////////
void HandleSpecialLaunchOptions( void )
{
    // Handle service install request from the installer
    if ( CommandLineContains( "/kdinstall" ) )
    {
        UpdateMTAVersionApplicationSetting( true );
        WatchDogReset();
        WatchDogBeginSection( WD_SECTION_POST_INSTALL );
        if ( CheckService( CHECK_SERVICE_POST_INSTALL ) )
            return ExitProcess( EXIT_OK );
        return ExitProcess( EXIT_ERROR );
    }

    // Handle service uninstall request from the installer
    if ( CommandLineContains( "/kduninstall" ) )
    {
        UpdateMTAVersionApplicationSetting( true );
        if ( CheckService( CHECK_SERVICE_PRE_UNINSTALL ) )
            return ExitProcess( EXIT_OK );
        return ExitProcess( EXIT_ERROR );
    }

    // No run 4 sure check
    if ( CommandLineContains( "/nolaunch" ) )
    {
        return ExitProcess( EXIT_OK );
    }
}


//////////////////////////////////////////////////////////
//
// HandleDuplicateLaunching
//
// Handle duplicate launching, or running from mtasa:// URI ?
//
//////////////////////////////////////////////////////////
void HandleDuplicateLaunching( void )
{
    LPSTR lpCmdLine = GetCommandLine();

    int iRecheckTimeLimit = 2000;
    while ( ! CreateSingleInstanceMutex () )
    {
        if ( strcmp ( lpCmdLine, "" ) != 0 )
        {
            HWND hwMTAWindow = FindWindow( NULL, "MTA: San Andreas" );
#ifdef MTA_DEBUG
            if( hwMTAWindow == NULL )
                hwMTAWindow = FindWindow( NULL, "MTA: San Andreas [DEBUG]" );
#endif
            if( hwMTAWindow != NULL )
            {
                LPWSTR szCommandLine = GetCommandLineW ();
                int numArgs;
                LPWSTR* aCommandLineArgs = CommandLineToArgvW ( szCommandLine, &numArgs );
                for ( int i = 1; i < numArgs; ++i )
                {
                    if ( WStringX( aCommandLineArgs[i] ).BeginsWith( L"mtasa://" ) )
                    {
                        WString wideConnectInfo = aCommandLineArgs[i];
                        SString strConnectInfo = ToUTF8 ( wideConnectInfo );

                        COPYDATASTRUCT cdStruct;
                        cdStruct.cbData = strConnectInfo.length () + 1;
                        cdStruct.lpData = const_cast<char *> ( strConnectInfo.c_str () );
                        cdStruct.dwData = URI_CONNECT;

                        SendMessage( hwMTAWindow, WM_COPYDATA, NULL, (LPARAM)&cdStruct );
                        break;
                    }
                }

                
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
                strMessage += _(    "Trouble restarting MTA:SA\n\n"
                                    "If the problem persists, open Task Manager and\n"
                                    "stop the 'gta_sa.exe' and 'Multi Theft Auto.exe' processes\n\n\n"
                                    "Try to launch MTA:SA again?" );
                if ( MessageBoxUTF8( 0, strMessage, _("Error")+_E("CL04"), MB_ICONWARNING | MB_YESNO | MB_TOPMOST  ) == IDYES ) // Trouble restarting MTA:SA
                {
                    TerminateGTAIfRunning ();
                    TerminateOtherMTAIfRunning ();
                    ShellExecuteNonBlocking( "open", PathJoin ( GetMTASAPath (), MTA_EXE_NAME ), lpCmdLine );
                }
                return ExitProcess( EXIT_ERROR );
            }
        }
        else
        {
            if ( !IsGTARunning () && !IsOtherMTARunning () )
            {
                MessageBoxUTF8 ( 0, _("Another instance of MTA is already running.\n\nIf this problem persists, please restart your computer"), _("Error")+_E("CL05"), MB_ICONERROR | MB_TOPMOST  );
            }
            else
            if ( MessageBoxUTF8( 0, _("Another instance of MTA is already running.\n\nDo you want to terminate it?"), _("Error")+_E("CL06"), MB_ICONQUESTION | MB_YESNO | MB_TOPMOST  ) == IDYES )
            {
                TerminateGTAIfRunning ();
                TerminateOtherMTAIfRunning ();
                ShellExecuteNonBlocking( "open", PathJoin ( GetMTASAPath (), MTA_EXE_NAME ), lpCmdLine );
            }
        }
        return ExitProcess( EXIT_ERROR );
    }
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

    int iResponse = MessageBoxUTF8 ( NULL, _("Are you having problems running MTA:SA?.\n\nDo you want to revert to an earlier version?"), "MTA: San Andreas"+_E("CL07"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST );
    if ( iResponse == IDYES )
    {
        BrowseToSolution ( "crashing-before-gtagame", TERMINATE_PROCESS );
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

    CheckAndShowMissingFileMessage();

    SString strSaveFilePath = PathJoin ( GetSystemPersonalPath(), "GTA San Andreas User Files" );
    SString strSettingsFilename = PathJoin ( strSaveFilePath, "gta_sa.set" );
    SString strSettingsFilenameBak = PathJoin ( strSaveFilePath, "gta_sa_old.set" );

    if ( FileExists ( strSettingsFilename ) )
    {
        int iResponse = MessageBoxUTF8 ( NULL, _("There seems to be a problem launching MTA:SA.\nResetting GTA settings can sometimes fix this problem.\n\nDo you want to reset GTA settings now?"), "MTA: San Andreas"+_E("CL08"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST );
        if ( iResponse == IDYES )
        {
            FileDelete ( strSettingsFilenameBak );
            FileRename ( strSettingsFilename, strSettingsFilenameBak );
            FileDelete ( strSettingsFilename );
            if ( !FileExists ( strSettingsFilename ) )
            {
                AddReportLog ( 4053, "Deleted gta_sa.set" );
                MessageBoxUTF8 ( NULL, _("GTA settings have been reset.\n\nPress OK to continue."), "MTA: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST );
            }
            else
            {
                AddReportLog ( 5054, SString ( "Delete gta_sa.set failed with '%s'", *strSettingsFilename ) );
                MessageBoxUTF8 ( NULL, SString ( _("File could not be deleted: '%s'"), *strSettingsFilename ), "Error"+_E("CL09"), MB_OK | MB_ICONWARNING | MB_TOPMOST );
            }
        }
    }
    else
    {
        // No settings to delete, or can't find them
        int iResponse = MessageBoxUTF8 ( NULL, _("Are you having problems running MTA:SA?.\n\nDo you want to see some online help?"), "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST );
        if ( iResponse == IDYES )
        {
            BrowseToSolution ( "crashing-before-gtalaunch", TERMINATE_PROCESS );
        }
    }
}


//////////////////////////////////////////////////////////
//
// HandleNotUsedMainMenu
//
// Called when a problem occured before the main menu was used by user
// If fullscreen, then maybe change fullscreen mode
//
//////////////////////////////////////////////////////////
void HandleNotUsedMainMenu ( void )
{
    AddReportLog( 9310, "Loader - HandleNotUsedMainMenu" );
    {
        // Slighty hacky way of checking in-game settings
        SString strCoreConfigFilename = CalcMTASAPath( PathJoin( "mta", "config", "coreconfig.xml" ) );
        SString strCoreConfig;
        FileLoad( strCoreConfigFilename, strCoreConfig );
        SString strWindowed        = strCoreConfig.SplitRight( "<display_windowed>" ).Left( 1 );
        SString strFullscreenStyle = strCoreConfig.SplitRight( "<display_fullscreen_style>" ).Left( 1 );
        if ( strFullscreenStyle == "1" )
        {
            AddReportLog( 9315, "Loader - HandleNotUsedMainMenu - Already Borderless window" );
        }
        else
        if ( !strWindowed.empty() && !strFullscreenStyle.empty())
        {
            if ( strWindowed == "0" && strFullscreenStyle == "0" )   // 0=FULLSCREEN_STANDARD
            {
                // Inform user
                SString strMessage = _("Are you having problems running MTA:SA?.\n\nDo you want to change the following setting?");
                strMessage += "\n" + _("Fullscreen mode:") + " -> " + _("Borderless window");
                HideSplash();
                int iResponse = MessageBoxUTF8 ( NULL, strMessage, "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST );
                if ( iResponse == IDYES )
                {
                    // Very hacky way of changing in-game settings
                    strCoreConfig = strCoreConfig.Replace( "<display_fullscreen_style>0", "<display_fullscreen_style>1" );
                    FileSave( strCoreConfigFilename, strCoreConfig );
                    AddReportLog( 9311, "Loader - HandleNotUsedMainMenu - User change to Borderless window" );
                }
                else
                    AddReportLog( 9313, "Loader - HandleNotUsedMainMenu - User said no" );
            }
            else
                AddReportLog( 9314, "Loader - HandleNotUsedMainMenu - Mode not fullscreen standard" );
        }
        else
        {
            // If no valid settings file yet, do the change without asking
            strCoreConfig = "<mainconfig><settings><display_fullscreen_style>1</display_fullscreen_style></settings></mainconfig>";
            FileSave( strCoreConfigFilename, strCoreConfig );
            AddReportLog( 9312, "Loader - HandleNotUsedMainMenu - Set Borderless window" );
        }
    }

    // Check if Evolve is active
    for ( auto processId : MyEnumProcesses( true ) )
    {
        SString strFilename = ExtractFilename( GetProcessPathFilename( processId ) );
        if ( strFilename.BeginsWithI( "Evolve" ) )
        {
            SString strMessage = _("Are you having problems running MTA:SA?.\n\nTry disabling the following products for GTA and MTA:");
            strMessage += "\n\nEvolve";
            DisplayErrorMessageBox ( strMessage, _E("CL43"), "not-used-menu-evolve" );
            break;
        }
    }
}


//////////////////////////////////////////////////////////
//
// HandleCustomStartMessage
//
//
//
//////////////////////////////////////////////////////////
void HandleCustomStartMessage ( void )
{
    SString strStartMessage = GetApplicationSetting( "diagnostics", "start-message" );
    SString strTrouble = GetApplicationSetting( "diagnostics", "start-message-trouble" );

    if ( strStartMessage.empty() )
        return;

    SetApplicationSetting( "diagnostics", "start-message", "" );
    SetApplicationSetting( "diagnostics", "start-message-trouble", "" );

    if ( strStartMessage.BeginsWith( "vdetect" ) )
    {
        SString strFilename = strStartMessage.SplitRight( "name=" );
        strStartMessage = _( "WARNING\n\n"
                             "MTA:SA has detected unusual activity.\n"
                             "Please run a virus scan to ensure your system is secure.\n\n" );
        strStartMessage += SString( _( "The detected file was:  %s\n" ), *strFilename );
    }

    DisplayErrorMessageBox( strStartMessage, _E("CL37"), strTrouble );
}


//////////////////////////////////////////////////////////
//
// PreLaunchWatchDogs
//
//
//
//////////////////////////////////////////////////////////
void PreLaunchWatchDogs ( void )
{
    assert ( !CreateSingleInstanceMutex () );

    //
    // "L0" is opened before the launch sequence and is closed if MTA shutsdown with no error
    // "L1" is opened before the launch sequence and is closed if GTA is succesfully started
    // "CR1" is a counter which is incremented if GTA was not started and MTA shutsdown with an error
    //
    // "L2" is opened before the launch sequence and is closed if the GTA loading screen is shown
    // "CR2" is a counter which is incremented at startup, if the previous run didn't make it to the loading screen
    //
    // "L3" is opened before the launch sequence and is closed if the GTA loading screen is shown, or a startup problem is handled elsewhere
    //

    // Check for unclean stop on previous run
#ifndef MTA_DEBUG
    if ( WatchDogIsSectionOpen ( "L0" ) )
        WatchDogSetUncleanStop ( true );    // Flag to maybe do things differently if MTA exit code on last run was not 0
    else
#endif
        WatchDogSetUncleanStop ( false );

    SString strCrashFlagFilename = CalcMTASAPath( "mta\\core.log.flag" );
    if ( FileExists( strCrashFlagFilename ) )
    {
        FileDelete( strCrashFlagFilename );
        WatchDogSetLastRunCrash( true );    // Flag to maybe do things differently if MTA crashed last run
    }
    else
        WatchDogSetLastRunCrash( false );

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

    // Check for possible fullscreen problems
    if ( WatchDogIsSectionOpen( WD_SECTION_NOT_USED_MAIN_MENU ) )
    {
        int iChainLimit;
        if ( WatchDogIsSectionOpen( WD_SECTION_POST_INSTALL ) )
            iChainLimit = 1;
        else
        if ( GetApplicationSettingInt( "times-connected" ) == 0 )
            iChainLimit = 2;
        else
            iChainLimit = 3;
        WatchDogCompletedSection( WD_SECTION_NOT_USED_MAIN_MENU );
        WatchDogIncCounter( WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU );
        if ( WatchDogGetCounter( WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU ) >= iChainLimit )
        {
            WatchDogClearCounter( WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU );
            HandleNotUsedMainMenu();
        }
    }
    else
        WatchDogClearCounter( WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU );

    // Clear down freeze on quit detection
    WatchDogCompletedSection( "Q0" );

    WatchDogBeginSection ( "L0" );      // Gets closed if MTA exits with a return code of 0
    WatchDogBeginSection ( "L1" );      // Gets closed when online game has started
    SetApplicationSetting ( "diagnostics", "gta-fopen-fail", "" );
    SetApplicationSetting ( "diagnostics", "last-crash-reason", "" );
    SetApplicationSetting ( "diagnostics", "gta-fopen-last", "" );
}


//////////////////////////////////////////////////////////
//
// PostRunWatchDogs
//
//
//
//////////////////////////////////////////////////////////
void PostRunWatchDogs ( int iReturnCode )
{
    if ( iReturnCode == 0 )
    {
        WatchDogClearCounter ( "CR1" );
        WatchDogCompletedSection ( "L0" );
    }
}


//////////////////////////////////////////////////////////
//
// HandleIfGTAIsAlreadyRunning
//
// Check for and maybe stop a running GTA process
//
//////////////////////////////////////////////////////////
void HandleIfGTAIsAlreadyRunning( void )
{
    if ( IsGTARunning () )
    {
        if ( MessageBoxUTF8 ( 0, _("An instance of GTA: San Andreas is already running. It needs to be terminated before MTA:SA can be started. Do you want to do that now?"), _("Information")+_E("CL10"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST ) == IDYES )
        {
            TerminateGTAIfRunning ();
            if ( IsGTARunning () )
            {
                MessageBoxUTF8 ( 0, _("Unable to terminate GTA: San Andreas. If the problem persists, please restart your computer."), _("Information")+_E("CL11"), MB_OK | MB_ICONERROR | MB_TOPMOST );
                return ExitProcess( EXIT_ERROR );
            }       
        }
        else
            return ExitProcess( EXIT_OK );
    }
}


//////////////////////////////////////////////////////////
//
// ValidateGTAPath
//
// Check GTA path looks good
//
//////////////////////////////////////////////////////////
void ValidateGTAPath( void )
{
    // Get path to GTA
    ePathResult iResult = DiscoverGTAPath( true );
    if ( iResult == GAME_PATH_MISSING ) {
        DisplayErrorMessageBox ( _("Registry entries are missing. Please reinstall Multi Theft Auto: San Andreas."), _E("CL12"), "reg-entries-missing" );
        return ExitProcess( EXIT_ERROR );
    }
    else if ( iResult == GAME_PATH_UNICODE_CHARS ) {
        DisplayErrorMessageBox ( _("The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains only standard ASCII characters and reinstall Multi Theft Auto: San Andreas."), _E("CL13") );
        return ExitProcess( EXIT_ERROR );
    }
    else if ( iResult == GAME_PATH_STEAM ) {
        DisplayErrorMessageBox ( _("It appears you have a Steam version of GTA:SA, which is currently incompatible with MTASA.  You are now being redirected to a page where you can find information to resolve this issue."), _E("CL14") );
        BrowseToSolution ( "downgrade-steam" );
        return ExitProcess( EXIT_ERROR );
    }

    SString strGTAPath = GetGTAPath();

    // We can now set this
    SetCurrentDirectory ( strGTAPath );

    const SString strMTASAPath = GetMTASAPath ();
    if ( strGTAPath.Contains ( ";" ) || strMTASAPath.Contains ( ";" ) )
    {
        DisplayErrorMessageBox (_( "The path to your installation of 'MTA:SA' or 'GTA: San Andreas'\n"
                                   "contains a ';' (semicolon).\n\n"
                                   " If you experience problems when running MTA:SA,\n"
                                   " move your installation(s) to a path that does not contain a semicolon." ), _E("CL15"), "path-semicolon" );
    }
}


//////////////////////////////////////////////////////////
//
// CheckAntiVirusStatus
//
// Maybe warn user if no anti-virus running
//
//////////////////////////////////////////////////////////
void CheckAntiVirusStatus( void )
{
    // Get data from WMI
    std::vector < SString > enabledList;
    std::vector < SString > disabledList;
    GetWMIAntiVirusStatus( enabledList, disabledList );

    // Get status from WSC
    WSC_SECURITY_PROVIDER_HEALTH health = (WSC_SECURITY_PROVIDER_HEALTH)-1;
    if ( _WscGetSecurityProviderHealth )
    {
        _WscGetSecurityProviderHealth( WSC_SECURITY_PROVIDER_ANTIVIRUS, &health );
    }

    // Dump results
    SString strStatus( "AV health: %s (%d)", *EnumToString( health ), health );
    for ( uint i = 0 ; i < enabledList.size() ; i++ )
        strStatus += SString( " [Ena%d:%s]", i, *enabledList[i] );
    for ( uint i = 0 ; i < disabledList.size() ; i++ )
        strStatus += SString( " [Dis%d:%s]", i, *disabledList[i] );
    WriteDebugEvent( strStatus ); 

    // Maybe show dialog if av not found
    if ( enabledList.empty() && health != WSC_SECURITY_PROVIDER_HEALTH_GOOD )
    {
        bool bEnableScaremongering = ( health != WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED );

        if ( bEnableScaremongering )
        {
            const char* avProducts[] = { "antivirus", "anti-virus", "Avast", "AVG", "Avira", 
                                         "NOD32", "ESET", "F-Secure", "Faronics", "Kaspersky",
                                         "McAfee", "Norton", "Symantec", "Panda", "Trend Micro", };

            // Check for anti-virus helper dlls before actual scaremongering
            HMODULE aModules[1024];
            DWORD cbNeeded;
            if ( EnumProcessModules( GetCurrentProcess(), aModules, sizeof(aModules), &cbNeeded) )
            {
                DWORD cModules = cbNeeded / sizeof(HMODULE);
                for ( uint i = 0 ; i < cModules ; i++ )
                {
                    if( aModules[i] != 0 )
                    {
                        WCHAR szModulePathFileName[1024] = L"";
                        GetModuleFileNameExW ( GetCurrentProcess(), aModules[i], szModulePathFileName, NUMELMS(szModulePathFileName) );
                        SLibVersionInfo libVersionInfo;
                        GetLibVersionInfo ( ToUTF8( szModulePathFileName ), &libVersionInfo );

                        for( uint i = 0 ; i < NUMELMS( avProducts ) ; i++ )
                        {
                            if ( libVersionInfo.strCompanyName.ContainsI( avProducts[i] )
                                 || libVersionInfo.strProductName.ContainsI( avProducts[i] ) )
                            {
                                bEnableScaremongering = false;
                                WriteDebugEvent( SString( "AV (module) maybe found %s [%d](%s,%s)", *WStringX( szModulePathFileName ).ToAnsi(), i, *libVersionInfo.strCompanyName, *libVersionInfo.strProductName ) );
                            }
                        }
                    }
                }

                if ( bEnableScaremongering )
                    WriteDebugEvent( SString( "AV Searched %d dlls, but could not find av helper", cModules ) );
            }

            if ( bEnableScaremongering )
            {
                for ( auto processId : MyEnumProcesses( true ) )
                {
                    SString strProcessPathFileName = GetProcessPathFilename ( processId );
                    if ( !strProcessPathFileName.empty() )
                    {
                        SLibVersionInfo libVersionInfo;
                        if ( GetLibVersionInfo ( strProcessPathFileName, &libVersionInfo ) )
                        {
                            for( uint i = 0 ; i < NUMELMS( avProducts ) ; i++ )
                            {
                                if ( libVersionInfo.strCompanyName.ContainsI( avProducts[i] )
                                     || libVersionInfo.strProductName.ContainsI( avProducts[i] ) )
                                {
                                    bEnableScaremongering = false;
                                    WriteDebugEvent( SString( "AV (process) maybe found %s [%d](%s,%s)", *strProcessPathFileName, i, *libVersionInfo.strCompanyName, *libVersionInfo.strProductName ) );
                                }
                            }
                        }
                    }
                }
                if ( bEnableScaremongering )
                    WriteDebugEvent( "AV Searched processes, but could not find av helper" );
            }
        }

        ShowNoAvDialog( g_hInstance, bEnableScaremongering );
        HideNoAvDialog ();
    }
}


//////////////////////////////////////////////////////////
//
// CheckDataFiles
//
// Basic check for some essential files
//
//////////////////////////////////////////////////////////
void CheckDataFiles( void )
{
    const SString strMTASAPath = GetMTASAPath();
    const SString strGTAPath = GetGTAPath();

    const char* dataFilesFiles [] = { "MTA\\cgui\\images\\background_logo.png"
                                     ,"MTA\\cgui\\images\\radarset\\up.png"
                                     ,"MTA\\cgui\\images\\busy_spinner.png"
                                     ,"MTA\\cgui\\images\\rect_edge.png"
                                     ,"MTA\\D3DX9_42.dll"
                                     ,"MTA\\D3DCompiler_42.dll"
                                     ,"MTA\\bass.dll"
                                     ,"MTA\\bass_fx.dll"
                                     ,"MTA\\tags.dll"
                                     ,"MTA\\sa.dat"
                                     ,"MTA\\XInput9_1_0_mta.dll"
                                     ,"MTA\\vea.dll"};

    for ( uint i = 0 ; i < NUMELMS( dataFilesFiles ) ; i++ )
    {
        if ( !FileExists ( PathJoin( strMTASAPath, dataFilesFiles [ i ] ) ) )
        {
            DisplayErrorMessageBox ( _("Load failed. Please ensure that the latest data files have been installed correctly."), _E("CL16"), "mta-datafiles-missing" );
            return ExitProcess( EXIT_ERROR );
        }
    }

    if ( FileSize ( PathJoin( strMTASAPath, "MTA", "bass.dll" ) ) != 0x0001A440 )
    {
        DisplayErrorMessageBox ( _("Load failed. Please ensure that the latest data files have been installed correctly."), _E("CL17"), "mta-datafiles-missing" );
        return ExitProcess( EXIT_ERROR );
    }

    // Check for client file
    if ( !FileExists ( PathJoin( strMTASAPath, CHECK_DM_CLIENT_NAME ) ) )
    {
        DisplayErrorMessageBox ( SString(_("Load failed. Please ensure that %s is installed correctly."),CHECK_DM_CLIENT_NAME), _E("CL18"), "client-missing" );
        return ExitProcess( EXIT_ERROR );
    }

    // Make sure the gta executable exists
    if ( !FileExists( PathJoin( strGTAPath, MTA_GTAEXE_NAME ) ) )
    {
        DisplayErrorMessageBox ( SString ( _("Load failed. Could not find gta_sa.exe in %s."), strGTAPath.c_str () ), _E("CL20"), "gta_sa-missing" );
        return ExitProcess( EXIT_ERROR );
    }

    // Make sure important dll's do not exist in the wrong place
    const char* dllCheckList[] = { "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll", "pthread.dll" };
    for ( int i = 0 ; i < NUMELMS ( dllCheckList ); i++ )
    {
        if ( FileExists( PathJoin( strGTAPath, dllCheckList[i] ) ) )
        {
            DisplayErrorMessageBox ( SString ( _("Load failed. %s exists in the GTA directory. Please delete before continuing."), dllCheckList[i] ), _E("CL21"), "file-clash" );
            return ExitProcess( EXIT_ERROR );
        }    
    }

    // Check main exe has the correct name
    if ( GetLaunchFilename().CompareI( MTA_EXE_NAME ) == false )
    {
        SString strMessage( _("Main file has an incorrect name (%s)"), *GetLaunchFilename() );
        int iResponse = MessageBoxUTF8 ( NULL, strMessage, _("Error")+_E("CL33"), MB_RETRYCANCEL | MB_ICONERROR | MB_TOPMOST  );
        ReleaseSingleInstanceMutex ();
        if ( iResponse == IDRETRY )
            ShellExecuteNonBlocking( "open", PathJoin ( strMTASAPath, MTA_EXE_NAME ) );            
        return ExitProcess( EXIT_ERROR );
    }

    // Check for possible virus file changing activities
    if ( !VerifyEmbeddedSignature( PathJoin( strMTASAPath, MTA_EXE_NAME ) ) )
    {
        SString strMessage( _("Main file is unsigned. Possible virus activity.\n\nSee online help if MTA does not work correctly.") );
        #if MTASA_VERSION_BUILD > 0 && defined(MTA_DM_CONNECT_TO_PUBLIC) && !defined(MTA_DEBUG)
            DisplayErrorMessageBox( strMessage, _E("CL29"), "maybe-virus1" );
        #endif
    }

    struct {
        const char* szMd5;
        const char* szFilename;
    } integrityCheckList[] = { { "9586E7BE6AE8016932038932D1417241", "bass.dll", },
                               { "B2E49F0C22C8B7D92D615F942BA19353", "bass_aac.dll", },
                               { "569C60F8397C34034E685A303B7404C0", "bass_ac3.dll", },
                               { "0E44BCAC0E940DB2BFB13448E96E4B29", "bass_fx.dll", },
                               { "50AF8A7D49E83A723ED0F70FB682DCFB", "bassflac.dll", },
                               { "BEBA64522AA8265751187E38D1FC0653", "bassmidi.dll", },
                               { "99F4F38007D347CEED482B7C04FDD122", "bassmix.dll", },
                               { "7B52BE6D702AA590DB57A0E135F81C45", "basswma.dll", }, 
                               { "38D7679D3B8B6D7F16A0AA9BF2A60043", "tags.dll", },
                               { "309D860FC8137E5FE9E7056C33B4B8BE", "vea.dll", },
                               { "0602F672BA595716E64EC4040E6DE376", "vog.dll", },
                               { "B37D7DF4A1430DB65AD3EA84801F9EC3", "vvo.dll", },
                               { "47FF3EE45DE53528F1AFD9F5982DF8C7", "vvof.dll", },
                               { "ADFB6D7B61E301761C700652B6FE7CCD", "XInput9_1_0_mta.dll", }, };
    for ( int i = 0 ; i < NUMELMS ( integrityCheckList ); i++ )
    {
        SString strMd5 = CMD5Hasher::CalculateHexString( PathJoin( strMTASAPath, "mta", integrityCheckList[i].szFilename ) );
        if ( !strMd5.CompareI( integrityCheckList[i].szMd5 ) )
        {
            DisplayErrorMessageBox( _("Data files modified. Possible virus activity.\n\nSee online help if MTA does not work correctly."), _E("CL30"), "maybe-virus2" );
            break;
        }    
    }

    // Check for asi files
    {
        bool bFoundInGTADir = !FindFiles( PathJoin( strGTAPath, "*.asi" ), true, false ).empty();
        bool bFoundInMTADir = !FindFiles( PathJoin( strMTASAPath, "mta", "*.asi" ), true, false ).empty();
        if ( bFoundInGTADir || bFoundInMTADir )
        {
            DisplayErrorMessageBox (_( ".asi files are in the 'MTA:SA' or 'GTA: San Andreas' installation directory.\n\n"
                                       "Remove these .asi files if you experience problems with MTA:SA." ), _E("CL28"), "asi-files" );
        }
    }

    // Warning if d3d9.dll exists in the GTA install directory
    if ( FileExists( PathJoin ( strGTAPath, "d3d9.dll" ) ) )
    {
        ShowD3dDllDialog ( g_hInstance, PathJoin ( strGTAPath, "d3d9.dll" ) );
        HideD3dDllDialog ();
    }

    // Remove old log files saved in the wrong place
    SString strMtaDir = PathJoin( strMTASAPath, "mta" );
    if ( strGTAPath.CompareI( strMtaDir ) == false )
    {
        FileDelete( PathJoin( strGTAPath, "CEGUI.log" ) );
        FileDelete( PathJoin( strGTAPath, "logfile.txt" ) );
        FileDelete( PathJoin( strGTAPath, "shutdown.log" ) );
    }
}


//////////////////////////////////////////////////////////
//
// CheckLibVersions
//
// Ensure DLLs are the correct version
//
//////////////////////////////////////////////////////////
void CheckLibVersions( void )
{
#if MTASA_VERSION_TYPE == VERSION_TYPE_RELEASE

    const char* moduleList [] =     { "MTA\\loader.dll"
                                     ,"MTA\\cgui.dll"
                                     ,"MTA\\core.dll"
                                     ,"MTA\\game_sa.dll"
                                     ,"MTA\\multiplayer_sa.dll"
                                     ,"MTA\\netc.dll"
                                     ,"MTA\\xmll.dll"
                                     ,"MTA\\game_sa.dll"
                                     ,"mods\\deathmatch\\client.dll"
                                     ,"mods\\deathmatch\\pcre3.dll"
                                    };
    SString strReqFileVersion;
    for ( uint i = 0 ; i < NUMELMS( moduleList ) ; i++ )
    {
        SString strFilename = moduleList[i];
#ifdef MTA_DEBUG
        strFilename = ExtractBeforeExtension( strFilename ) + "_d." + ExtractExtension( strFilename );
#endif
        SLibVersionInfo fileInfo;
        if ( FileExists( CalcMTASAPath( strFilename ) ) )
        {
            SString strFileVersion = "0.0.0.0";
            if ( GetLibVersionInfo( CalcMTASAPath( strFilename ), &fileInfo ) )
                strFileVersion = SString( "%d.%d.%d.%d", fileInfo.dwFileVersionMS >> 16, fileInfo.dwFileVersionMS & 0xFFFF
                                                       , fileInfo.dwFileVersionLS >> 16, fileInfo.dwFileVersionLS & 0xFFFF );
            if ( strReqFileVersion.empty() )
                strReqFileVersion = strFileVersion;
            else
            if ( strReqFileVersion != strFileVersion )
            {
                DisplayErrorMessageBox ( SStringX(_( "File version mismatch error."
                                            " Reinstall MTA:SA if you experience problems.\n" )
                                            + SString( "\n[%s %s/%s]\n", *strFilename, *strFileVersion, *strReqFileVersion )
                                            ), _E("CL40"), "bad-file-version" );
                break;
            }
        }
        else
        {
            DisplayErrorMessageBox ( SStringX(_( "Some files are missing."
                                        " Reinstall MTA:SA if you experience problems.\n" )
                                        + SString( "\n[%s]\n", *strFilename )
                                        ), _E("CL41"), "missing-file" );
            break;
        }
    }

#endif

    if ( GetSystemMetrics( SM_CLEANBOOT ) != 0 )
    {
        DisplayErrorMessageBox ( SStringX(_( "MTA:SA is not compatible with Windows 'Safe Mode'.\n\n"
                                    "Please restart your PC.\n" )
                                    ), _E("CL42"), "safe-mode" );
        return ExitProcess( EXIT_ERROR );
    }
}


//////////////////////////////////////////////////////////
//
// LaunchGame
//
// Create GTA process, hook it and go go go
//
//////////////////////////////////////////////////////////
int LaunchGame ( SString strCmdLine )
{
    CheckAndShowModelProblems();
    CheckAndShowUpgradeProblems();

    const SString strGTAPath = GetGTAPath ();
    const SString strMTASAPath = GetMTASAPath ();
    SString strMtaDir = PathJoin( strMTASAPath, "mta" );

    SetDllDirectory( strMtaDir );
    CheckService ( CHECK_SERVICE_PRE_CREATE );

    // Do some D3D things
    BeginD3DStuff();
    LogSettings();

    // Use renamed exe if required
    SString strGTAEXEPath = GetInstallManager()->MaybeRenameExe( strGTAPath );
    SetCurrentDirectory ( strGTAPath );

    //////////////////////////////////////////////////////////
    //
    // Hook 'n' go
    //
    // Launch GTA using CreateProcess
    PROCESS_INFORMATION piLoadee;
    STARTUPINFOW siLoadee;
    memset( &piLoadee, 0, sizeof ( PROCESS_INFORMATION ) );
    memset( &siLoadee, 0, sizeof ( STARTUPINFO ) );
    siLoadee.cb = sizeof ( STARTUPINFO );

    WatchDogBeginSection ( "L2" );      // Gets closed when loading screen is shown
    WatchDogBeginSection ( "L3" );      // Gets closed when loading screen is shown, or a startup problem is handled elsewhere
    WatchDogBeginSection ( WD_SECTION_NOT_USED_MAIN_MENU );      // Gets closed when the main menu is used

    // Extract 'done-admin' flag from command line
    bool bDoneAdmin = strCmdLine.Contains ( "/done-admin" );
    strCmdLine = strCmdLine.Replace ( " /done-admin", "" );

    // Add server connection after update to command line
    SString strPostUpdateConnect = GetPostUpdateConnect();
    if ( !strPostUpdateConnect.empty() && strCmdLine.empty() )
        strCmdLine = SString( "mtasa://%s", *strPostUpdateConnect );

    WString wstrCmdLine = FromUTF8( strCmdLine );

    // Start GTA
    if ( 0 == _CreateProcessW( FromUTF8( strGTAEXEPath ),
                              (LPWSTR)*wstrCmdLine,
                              NULL,
                              NULL,
                              FALSE,
                              CREATE_SUSPENDED,
                              NULL,
                              FromUTF8( strMtaDir ),    //    strMTASAPath\mta is used so pthreadVC2.dll can be found
                              &siLoadee,
                              &piLoadee ) )
    {
        DWORD dwError = GetLastError ();
        WriteDebugEvent( SString( "Loader - Process not created[%d]: %s", dwError, *strGTAEXEPath ) );

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
            DisplayErrorMessageBox ( SString(_("Could not start Grand Theft Auto: San Andreas.  "
                                "Please try restarting, or if the problem persists,"
                                "contact MTA at www.multitheftauto.com. \n\n[%s]"),*strError), _E("CL22"), "createprocess-fail&err=" + strError ); // Could not start GTA:SA
            return 5;
        }
    }

    WriteDebugEvent( SString( "Loader - Process created: %s %s", *strGTAEXEPath, *GetApplicationSetting( "serial" ) ) );
    
    // Inject the core into GTA
    SetDllDirectory( strMtaDir );
    SString strCoreDLL = PathJoin( strMTASAPath, "mta", MTA_DLL_NAME );
    RemoteLoadLibrary ( piLoadee.hProcess, FromUTF8( strCoreDLL ) );
    WriteDebugEvent( SString( "Loader - Core injected: %s", *strCoreDLL ) );
    AddReportLog( 7103, "Loader - Core injected" );

    // Clear previous on quit commands
    SetOnQuitCommand ( "" );

    ShowSplash( g_hInstance );  // Bring splash to the front

    // Resume execution for the game.
    ResumeThread ( piLoadee.hThread );

    if ( piLoadee.hThread)
    {
        WriteDebugEvent( "Loader - Waiting for L3 to close" );

        BsodDetectionOnGameBegin();
        // Show splash until game window is displayed (or max 20 seconds)
        DWORD status;
        bool bShownDeviceSelectionDialog = false;
        for ( uint i = 0 ; i < 20 ; i++ )
        {
            status = WaitForSingleObject ( piLoadee.hProcess, 1000 );
            if ( status != WAIT_TIMEOUT )
                break;

            if ( !WatchDogIsSectionOpen( "L3" ) )     // Gets closed when loading screen is shown
            {
                WriteDebugEvent( "Loader - L3 closed" );
                break;
            }

            // Skip stuck process warning if DeviceSelection dialog is still open after 4 seconds
            if ( i >= 4 )
                bShownDeviceSelectionDialog |= IsDeviceSelectionDialogOpen( piLoadee.dwThreadId );
        }

        // Actually hide the splash
        HideSplash ();

        // If hasn't shown the loading screen and gta_sa.exe process memory usage is not changing, give user option to terminate
        if ( status == WAIT_TIMEOUT && !bShownDeviceSelectionDialog )
        {
            CStuckProcessDetector stuckProcessDetector( piLoadee.hProcess, 5000 );
            while ( status == WAIT_TIMEOUT && WatchDogIsSectionOpen( "L3" ) )     // Gets closed when loading screen is shown
            {
                if ( stuckProcessDetector.UpdateIsStuck() )
                {
                    WriteDebugEvent( "Detected stuck process at startup" );
                    if ( MessageBoxUTF8 ( 0, _("GTA: San Andreas may not have launched correctly. Do you want to terminate it?"), _("Information")+_E("CL25"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST ) == IDYES )
                    {
                        WriteDebugEvent( "User selected process termination" );
                        TerminateProcess ( piLoadee.hProcess, 1 );
                    }
                    break;
                }
                status = WaitForSingleObject( piLoadee.hProcess, 1000 );
            }
        }

        // Wait for game to exit
        WriteDebugEvent( "Loader - Wait for game to exit" );
        while ( status == WAIT_TIMEOUT )
        {
            status = WaitForSingleObject( piLoadee.hProcess, 1500 );

            // If core is closing and gta_sa.exe process memory usage is not changing, terminate
            CStuckProcessDetector stuckProcessDetector( piLoadee.hProcess, 5000 );
            while ( status == WAIT_TIMEOUT && WatchDogIsSectionOpen( "Q0" ) )     // Gets closed when quit has completed
            {
                if ( stuckProcessDetector.UpdateIsStuck() )
                {
                    WriteDebugEvent( "Detected stuck process at quit" );
                #ifndef MTA_DEBUG
                    TerminateProcess( piLoadee.hProcess, 1 );
                    status = WAIT_FAILED;
                    break;
                #endif
                }
                status = WaitForSingleObject( piLoadee.hProcess, 1000 );
            }
        }

        BsodDetectionOnGameEnd();
    }

    AddReportLog( 7104, "Loader - Finishing" );
    WriteDebugEvent( "Loader - Finishing" );

    EndD3DStuff();

    // Get its exit code
    DWORD dwExitCode = -1;
    GetExitCodeProcess( piLoadee.hProcess, &dwExitCode );

    // Terminate to be sure
    TerminateProcess ( piLoadee.hProcess, 1 );

    //////////////////////////////////////////////////////////
    //
    // On exit
    //
    // Cleanup and exit.
    CloseHandle ( piLoadee.hProcess );
    CloseHandle ( piLoadee.hThread );
    ReleaseSingleInstanceMutex ();

    // Success, maybe
    return dwExitCode;
}


//////////////////////////////////////////////////////////
//
// HandleOnQuitCommand
//
// Parse and do something with OnQuitCommand from registry
//
//////////////////////////////////////////////////////////
void HandleOnQuitCommand( void )
{
    const SString strMTASAPath = GetMTASAPath ();
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
            strFile = PathJoin( strMTASAPath, MTA_EXE_NAME );
        }
        else
            CheckService ( CHECK_SERVICE_POST_GAME );     // Stop service here if quit command is not 'restart'

        LPCTSTR lpOperation     = strOperation == "" ? NULL : strOperation.c_str ();
        LPCTSTR lpFile          = strFile.c_str ();
        LPCTSTR lpParameters    = strParameters == "" ? NULL : strParameters.c_str ();
        LPCTSTR lpDirectory     = strDirectory == "" ? NULL : strDirectory.c_str ();
        INT nShowCmd            = strShowCmd == "" ? SW_SHOWNORMAL : atoi( strShowCmd );

        if ( lpOperation && lpFile )
        {
            // Maybe spawn an exe
            ShellExecuteNonBlocking( lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );            
        }
    }
    else
        CheckService ( CHECK_SERVICE_POST_GAME );     // Stop service here if quit command is empty
}
