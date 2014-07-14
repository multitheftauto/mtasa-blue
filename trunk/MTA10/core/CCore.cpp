/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCore.cpp
*  PURPOSE:     Base core class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <Accctrl.h>
#include <Aclapi.h>
#include "Userenv.h"        // This will enable SharedUtil::ExpandEnvString
#define ALLOC_STATS_MODULE_NAME "core"
#include "SharedUtil.hpp"
#include <clocale>
#include "CTimingCheckpoints.hpp"
#include "CModelCacheManager.h"

using SharedUtil::CalcMTASAPath;
using namespace std;

static float fTest = 1;

extern CCore* g_pCore;
bool g_bBoundsChecker = false;

BOOL AC_RestrictAccess( VOID )
{
    EXPLICIT_ACCESS NewAccess;
    PACL pTempDacl;
    HANDLE hProcess;
    DWORD dwFlags;
    DWORD dwErr;

    ///////////////////////////////////////////////
    // Get the HANDLE to the current process.
    hProcess = GetCurrentProcess();

    ///////////////////////////////////////////////
    // Setup which accesses we want to deny.
    dwFlags = GENERIC_WRITE|PROCESS_ALL_ACCESS|WRITE_DAC|DELETE|WRITE_OWNER|READ_CONTROL;

    ///////////////////////////////////////////////
    // Build our EXPLICIT_ACCESS structure.
    BuildExplicitAccessWithName( &NewAccess, "CURRENT_USER", dwFlags, DENY_ACCESS, NO_INHERITANCE ); 


    ///////////////////////////////////////////////
    // Create our Discretionary Access Control List.
    if ( ERROR_SUCCESS != (dwErr = SetEntriesInAcl( 1, &NewAccess, NULL, &pTempDacl )) )
    {
        #ifdef DEBUG
//        pConsole->Con_Printf("Error at SetEntriesInAcl(): %i", dwErr);
        #endif
        return FALSE;
    }

    ////////////////////////////////////////////////
    // Set the new DACL to our current process.
    if ( ERROR_SUCCESS != (dwErr = SetSecurityInfo( hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pTempDacl, NULL )) )
    {
        #ifdef DEBUG
//        pConsole->Con_Printf("Error at SetSecurityInfo(): %i", dwErr);
        #endif
        return FALSE;
    }

    ////////////////////////////////////////////////
    // Free the DACL (see msdn on SetEntriesInAcl)
    LocalFree( pTempDacl );
    CloseHandle( hProcess );

    return TRUE;
}


template<> CCore * CSingleton< CCore >::m_pSingleton = NULL;

CCore::CCore ( void )
{
    // Initialize the global pointer
    g_pCore = this;

    #if !defined(MTA_DEBUG) && !defined(MTA_ALLOW_DEBUG)
    AC_RestrictAccess ();
    #endif
    
    m_pConfigFile = NULL;

    // Set our locale to the C locale, except for character handling which is the system's default
    std::setlocale(LC_ALL,"C");
    std::setlocale(LC_CTYPE,"");
    // check LC_COLLATE is the old-time raw ASCII sort order
    assert ( strcoll( "a", "B" ) > 0 );

    // Parse the command line
    const char* pszNoValOptions[] =
    {
        "window",
        NULL
    };
    ParseCommandLine ( m_CommandLineOptions, m_szCommandLineArgs, pszNoValOptions );

    // Load our settings and localization as early as possible
    CreateXML ( );
    g_pLocalization = new CLocalization;

    // Create a logger instance.
    m_pConsoleLogger            = new CConsoleLogger ( );

    // Create interaction objects.
    m_pCommands                 = new CCommands;
    m_pConnectManager           = new CConnectManager;

    // Create the GUI manager and the graphics lib wrapper
    m_pLocalGUI                 = new CLocalGUI;
    m_pGraphics                 = new CGraphics ( m_pLocalGUI );
    g_pGraphics                 = m_pGraphics;
    m_pGUI                      = NULL;

    // Create the mod manager
    m_pModManager               = new CModManager;

    m_pfnMessageProcessor       = NULL;
    m_pMessageBox = NULL;

    m_bFirstFrame = true;
    m_bIsOfflineMod = false;
    m_bQuitOnPulse = false;
    m_bDestroyMessageBox = false;
    m_bCursorToggleControls = false;
    m_bLastFocused = true;
    m_bWaitToSetNick = false;
    m_DiagnosticDebug = EDiagnosticDebug::NONE;

    // Create our Direct3DData handler.
    m_pDirect3DData = new CDirect3DData;

    WriteDebugEvent ( "CCore::CCore" );

    m_pKeyBinds = new CKeyBinds ( this );

    m_pMouseControl = new CMouseControl();

    // Create our hook objects.
    //m_pFileSystemHook           = new CFileSystemHook ( );
    m_pDirect3DHookManager      = new CDirect3DHookManager ( );
    m_pDirectInputHookManager   = new CDirectInputHookManager ( );
    m_pMessageLoopHook          = new CMessageLoopHook ( );
    m_pSetCursorPosHook         = new CSetCursorPosHook ( );
    m_pTCPManager               = new CTCPManager ( );

    // Register internal commands.
    RegisterCommands ( );

    // Setup our hooks.
    ApplyHooks ( );

    // Reset the screenshot flag
    bScreenShot = false;

    //Create our current server and set the update time to zero
    m_pCurrentServer = new CXfireServerInfo();
    m_tXfireUpdate = 0;

    // No initial fps limit
    m_bDoneFrameRateLimit = false;
    m_uiFrameRateLimit = 0;
    m_uiServerFrameRateLimit = 0;
    m_uiNewNickWaitFrames = 0;
    m_iUnminimizeFrameCounter = 0;
    m_bDidRecreateRenderTargets = false;
    m_fMinStreamingMemory = 0;
    m_fMaxStreamingMemory = 0;
    m_bGettingIdleCallsFromMultiplayer = false;
    m_bWindowsTimerEnabled = false;
}

CCore::~CCore ( void )
{
    WriteDebugEvent ( "CCore::~CCore" );

    // Delete the mod manager
    delete m_pModManager;
    SAFE_DELETE ( m_pMessageBox );

    // Destroy early subsystems
    m_bModulesLoaded = false;
    DestroyNetwork ();
    DestroyMultiplayer ();
    DestroyGame ();

    // Remove global events
    g_pCore->m_pGUI->ClearInputHandlers( INPUT_CORE );

    // Remove input hook
    CMessageLoopHook::GetSingleton ( ).RemoveHook ( );

    // Store core variables to cvars
    CVARS_SET ( "console_pos",                  m_pLocalGUI->GetConsole ()->GetPosition () );
    CVARS_SET ( "console_size",                 m_pLocalGUI->GetConsole ()->GetSize () );

    // Delete interaction objects.
    delete m_pCommands;
    delete m_pConnectManager;
    delete m_pDirect3DData;

    // Delete hooks.
    delete m_pSetCursorPosHook;
    //delete m_pFileSystemHook;
    delete m_pDirect3DHookManager;
    delete m_pDirectInputHookManager;
    delete m_pTCPManager;

    // Delete the GUI manager    
    delete m_pLocalGUI;
    delete m_pGraphics;

    // Delete lazy subsystems
    DestroyGUI ();
    DestroyXML ();

    // Delete keybinds
    delete m_pKeyBinds;

    // Delete Mouse Control
    delete m_pMouseControl;

    // Delete the logger
    delete m_pConsoleLogger;

    //Delete the Current Server
    delete m_pCurrentServer;

    // Delete last so calls to GetHookedWindowHandle do not crash
    delete m_pMessageLoopHook;
}


eCoreVersion CCore::GetVersion ( void )
{
    return MTACORE_20;
}


CConsoleInterface* CCore::GetConsole ( void )
{
    return m_pLocalGUI->GetConsole ();
}


CCommandsInterface* CCore::GetCommands ( void )
{
    return m_pCommands;
}


CGame* CCore::GetGame ( void )
{
    return m_pGame;
}


CGraphicsInterface* CCore::GetGraphics ( void )
{
    return m_pGraphics;
}


CModManagerInterface* CCore::GetModManager ( void )
{
    return m_pModManager;
}


CMultiplayer* CCore::GetMultiplayer ( void )
{
    return m_pMultiplayer;
}

CXMLNode* CCore::GetConfig ( void )
{
    if ( !m_pConfigFile )
        return NULL;
    CXMLNode* pRoot = m_pConfigFile->GetRootNode ();
    if ( !pRoot )
        pRoot = m_pConfigFile->CreateRootNode ( CONFIG_ROOT );
    return pRoot;
}

CGUI* CCore::GetGUI ( void )
{
    return m_pGUI;
}


CNet* CCore::GetNetwork ( void )
{
    return m_pNet;
}


CKeyBindsInterface* CCore::GetKeyBinds ( void )
{
    return m_pKeyBinds;
}


CLocalGUI* CCore::GetLocalGUI ( void )
{
    return m_pLocalGUI;
}


void CCore::SaveConfig ( void )
{
    if ( m_pConfigFile )
    {
        CXMLNode* pBindsNode = GetConfig ()->FindSubNode ( CONFIG_NODE_KEYBINDS );
        if ( !pBindsNode )
            pBindsNode = GetConfig ()->CreateSubNode ( CONFIG_NODE_KEYBINDS );
        m_pKeyBinds->SaveToXML ( pBindsNode );
        GetVersionUpdater ()->SaveConfigToXML ();
        GetServerCache ()->SaveServerCache ();
        m_pConfigFile->Write ();
    }
}

void CCore::ChatEcho ( const char* szText, bool bColorCoded )
{
    CChat* pChat = m_pLocalGUI->GetChat ();
    if ( pChat )
    {
        CColor color ( 255, 255, 255, 255 );
        pChat->SetTextColor ( color );
    }

    // Echo it to the console and chat
    m_pLocalGUI->EchoChat ( szText, bColorCoded );
    if ( bColorCoded )
    {
        m_pLocalGUI->EchoConsole ( RemoveColorCodes( szText ) );
    }
    else
        m_pLocalGUI->EchoConsole ( szText );
}


void CCore::DebugEcho ( const char* szText )
{
    CDebugView * pDebugView = m_pLocalGUI->GetDebugView ();
    if ( pDebugView )
    {
        CColor color ( 255, 255, 255, 255 );
        pDebugView->SetTextColor ( color );
    }

    m_pLocalGUI->EchoDebug ( szText );
}

void CCore::DebugPrintf ( const char* szFormat, ... )
{
    // Convert it to a string buffer
    char szBuffer [1024];
    va_list ap;
    va_start ( ap, szFormat );
    VSNPRINTF ( szBuffer, 1024, szFormat, ap );
    va_end ( ap );

    DebugEcho ( szBuffer );
}

void CCore::SetDebugVisible ( bool bVisible )
{
    if ( m_pLocalGUI )
    {
        m_pLocalGUI->SetDebugViewVisible ( bVisible );
    }
}


bool CCore::IsDebugVisible ( void )
{
    if ( m_pLocalGUI )
        return m_pLocalGUI->IsDebugViewVisible ();
    else
        return false;
}


void CCore::DebugEchoColor ( const char* szText, unsigned char R, unsigned char G, unsigned char B )
{
    // Set the color
    CDebugView * pDebugView = m_pLocalGUI->GetDebugView ();
    if ( pDebugView )
    {
        CColor color ( R, G, B, 255 );
        pDebugView->SetTextColor ( color );
    }

    m_pLocalGUI->EchoDebug ( szText );
}


void CCore::DebugPrintfColor ( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... )
{
    // Set the color
    if ( szFormat )
    {
        // Convert it to a string buffer
        char szBuffer [1024];
        va_list ap;
        va_start ( ap, B );
        VSNPRINTF ( szBuffer, 1024, szFormat, ap );
        va_end ( ap );

        // Echo it to the console and chat
        DebugEchoColor ( szBuffer, R, G, B );
    }
}


void CCore::DebugClear ( void )
{
    CDebugView * pDebugView = m_pLocalGUI->GetDebugView ();
    if ( pDebugView )
    {
        pDebugView->Clear();
    }
}


void CCore::ChatEchoColor ( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded )
{
    // Set the color
    CChat* pChat = m_pLocalGUI->GetChat ();
    if ( pChat )
    {
        CColor color ( R, G, B, 255 );
        pChat->SetTextColor ( color );
    }

    // Echo it to the console and chat
    m_pLocalGUI->EchoChat ( szText, bColorCoded );
    if ( bColorCoded )
    {
        m_pLocalGUI->EchoConsole ( RemoveColorCodes( szText ) );
    }
    else
        m_pLocalGUI->EchoConsole ( szText );
}


void CCore::ChatPrintf ( const char* szFormat, bool bColorCoded, ... )
{
    // Convert it to a string buffer
    char szBuffer [1024];
    va_list ap;
    va_start ( ap, bColorCoded );
    VSNPRINTF ( szBuffer, 1024, szFormat, ap );
    va_end ( ap );

    // Echo it to the console and chat
    ChatEcho ( szBuffer, bColorCoded );
}


void CCore::ChatPrintfColor ( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... )
{
    // Set the color
    if ( szFormat )
    {
        if ( m_pLocalGUI )
        {
            // Convert it to a string buffer
            char szBuffer [1024];
            va_list ap;
            va_start ( ap, B );
            VSNPRINTF ( szBuffer, 1024, szFormat, ap );
            va_end ( ap );

            // Echo it to the console and chat
            ChatEchoColor ( szBuffer, R, G, B, bColorCoded );
        }
    }
}


void CCore::SetChatVisible ( bool bVisible )
{
    if ( m_pLocalGUI )
    {
        m_pLocalGUI->SetChatBoxVisible ( bVisible );
    }
}


bool CCore::IsChatVisible ( void )
{
    if ( m_pLocalGUI )
    {
        return m_pLocalGUI->IsChatBoxVisible ();
    }
    return false;
}


void CCore::TakeScreenShot ( void )
{
    bScreenShot = true;
}


void CCore::EnableChatInput ( char* szCommand, DWORD dwColor )
{
    if ( m_pLocalGUI )
    {
        if ( m_pGame->GetSystemState () == 9 /* GS_PLAYING_GAME */ &&
            m_pModManager->GetCurrentMod () != NULL &&
            !IsOfflineMod () &&
            !m_pGame->IsAtMenu () &&
            !m_pLocalGUI->GetMainMenu ()->IsVisible () &&
            !m_pLocalGUI->GetConsole ()->IsVisible () &&
            !m_pLocalGUI->IsChatBoxInputEnabled () )
        {
            CChat* pChat = m_pLocalGUI->GetChat ();
            pChat->SetCommand ( szCommand );
            m_pLocalGUI->SetChatBoxInputEnabled ( true );
        }
    }
}


bool CCore::IsChatInputEnabled ( void )
{
    if ( m_pLocalGUI )
    {
        return ( m_pLocalGUI->IsChatBoxInputEnabled () );
    }

    return false;
}


bool CCore::IsSettingsVisible ( void )
{
    if ( m_pLocalGUI )
    {
        return ( m_pLocalGUI->GetMainMenu ()->GetSettingsWindow ()->IsVisible () );
    }
    
    return false;
}


bool CCore::IsMenuVisible ( void )
{
    if ( m_pLocalGUI )
    {
        return ( m_pLocalGUI->GetMainMenu ()->IsVisible () );
    }

    return false;
}


bool CCore::IsCursorForcedVisible ( void )
{
    if ( m_pLocalGUI )
    {
        return ( m_pLocalGUI->IsCursorForcedVisible () );
    }

    return false;
}

void CCore::ApplyConsoleSettings ( void )
{
    CVector2D vec;
    CConsole * pConsole = m_pLocalGUI->GetConsole ();

    CVARS_GET ( "console_pos", vec );
    pConsole->SetPosition ( vec );
    CVARS_GET ( "console_size", vec );
    pConsole->SetSize ( vec );
}

void CCore::ApplyGameSettings ( void )
{
    bool bval;
    int iVal;
    CControllerConfigManager * pController = m_pGame->GetControllerConfigManager ();

    CVARS_GET ( "invert_mouse",     bval ); pController->SetMouseInverted ( bval );
    CVARS_GET ( "fly_with_mouse",   bval ); pController->SetFlyWithMouse ( bval );
    CVARS_GET ( "steer_with_mouse", bval ); pController->SetSteerWithMouse ( bval );
    CVARS_GET ( "classic_controls", bval ); pController->SetClassicControls ( bval );
    CVARS_GET ( "volumetric_shadows", bval ); m_pGame->GetSettings ()->SetVolumetricShadowsEnabled ( bval );
    CVARS_GET ( "aspect_ratio",     iVal ); m_pGame->GetSettings ()->SetAspectRatio ( (eAspectRatio)iVal, CVARS_GET_VALUE < bool > ( "hud_match_aspect_ratio" ) );
    CVARS_GET ( "grass",            bval ); m_pGame->GetSettings ()->SetGrassEnabled ( bval );
    CVARS_GET ( "heat_haze",        bval ); m_pMultiplayer->SetHeatHazeEnabled ( bval );
    CVARS_GET ( "fast_clothes_loading", iVal ); m_pMultiplayer->SetFastClothesLoading ( (CMultiplayer::EFastClothesLoading)iVal );
    pController->SetVerticalAimSensitivityRawValue( CVARS_GET_VALUE < float > ( "vertical_aim_sensitivity" ) );
}

void CCore::ApplyCommunityState ( void )
{
    bool bLoggedIn = g_pCore->GetCommunity()->IsLoggedIn();
    if ( bLoggedIn )
        m_pLocalGUI->GetMainMenu ()->GetSettingsWindow()->OnLoginStateChange ( true );
}

void CCore::SetConnected ( bool bConnected )
{
    m_pLocalGUI->GetMainMenu ( )->SetIsIngame ( bConnected );
    UpdateIsWindowMinimized ();  // Force update of stuff
}


bool CCore::IsConnected ( void )
{
    return m_pLocalGUI->GetMainMenu ( )->GetIsIngame ();
}


bool CCore::Reconnect ( const char* szHost, unsigned short usPort, const char* szPassword, bool bSave, bool bForceInternalHTTPServer )
{
    return m_pConnectManager->Reconnect ( szHost, usPort, szPassword, bSave, bForceInternalHTTPServer );
}


bool CCore::ShouldUseInternalHTTPServer( void )
{
    return m_pConnectManager->ShouldUseInternalHTTPServer();
}


void CCore::SetOfflineMod ( bool bOffline )
{
    m_bIsOfflineMod = bOffline;
}


const char* CCore::GetModInstallRoot ( const char* szModName )
{
    m_strModInstallRoot = CalcMTASAPath ( PathJoin ( "mods", szModName ) );
    return m_strModInstallRoot;
}


void CCore::ForceCursorVisible ( bool bVisible, bool bToggleControls )
{
    m_bCursorToggleControls = bToggleControls;
    m_pLocalGUI->ForceCursorVisible ( bVisible );
}


void CCore::SetMessageProcessor ( pfnProcessMessage pfnMessageProcessor )
{
    m_pfnMessageProcessor = pfnMessageProcessor;
}


void CCore::ShowMessageBox ( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler )
{
    if ( m_pMessageBox )
        delete m_pMessageBox;

    // Create the message box
    m_pMessageBox = m_pGUI->CreateMessageBox ( szTitle, szText, uiFlags );
    if ( ResponseHandler ) m_pMessageBox->SetClickHandler ( *ResponseHandler );

    // Make sure it doesn't auto-destroy, or we'll crash if the msgbox had buttons and the user clicks OK
    m_pMessageBox->SetAutoDestroy ( false );
}


void CCore::RemoveMessageBox ( bool bNextFrame )
{
    if ( bNextFrame )
    {
        m_bDestroyMessageBox = true;
    }
    else
    {
        if ( m_pMessageBox )
        {
            delete m_pMessageBox;
            m_pMessageBox = NULL;
        }
    }
}

//
// Show message box with possibility of on-line help
//
void CCore::ShowErrorMessageBox( const SString& strTitle, SString strMessage, const SString& strTroubleLink )
{
    if ( strTroubleLink.empty() )
    {
        CCore::GetSingleton ().ShowMessageBox ( strTitle, strMessage, MB_BUTTON_OK | MB_ICON_ERROR );
    }
    else
    {
        strMessage += "\n\n";
        strMessage += _("Do you want to see some on-line help about this problem ?");
        CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
        pQuestionBox->Reset ();
        pQuestionBox->SetTitle ( strTitle );
        pQuestionBox->SetMessage ( strMessage );
        pQuestionBox->SetButton ( 0, _("No") );
        pQuestionBox->SetButton ( 1, _("Yes") );
        pQuestionBox->SetCallback ( CCore::ErrorMessageBoxCallBack, new SString( strTroubleLink ) );
        pQuestionBox->Show ();
    }
}

//
// Show message box with possibility of on-line help
//  + with net error code appended to message and trouble link
//
void CCore::ShowNetErrorMessageBox( const SString& strTitle, SString strMessage, SString strTroubleLink, bool bLinkRequiresErrorCode )
{
    uint uiErrorCode = CCore::GetSingleton ().GetNetwork ()->GetExtendedErrorCode ();
    if ( uiErrorCode != 0 )
    {
        // Do anti-virus check soon
        SetApplicationSettingInt( "noav-user-says-skip", 1 );
        strMessage += SString ( " \nCode: %08X", uiErrorCode );
        if ( !strTroubleLink.empty() )
            strTroubleLink += SString ( "&neterrorcode=%08X", uiErrorCode );
    }
    else
    if ( bLinkRequiresErrorCode )
        strTroubleLink = "";        // No link if no error code

    ShowErrorMessageBox( strTitle, strMessage, strTroubleLink );
}

//
// Callback used in CCore::ShowErrorMessageBox
//
void CCore::ErrorMessageBoxCallBack( void* pData, uint uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();

    SString* pstrTroubleLink = (SString*)pData;
    if ( uiButton == 1 )
    {
        uint uiErrorCode = (uint)pData;
        BrowseToSolution ( *pstrTroubleLink, EXIT_GAME_FIRST );
    }
    delete pstrTroubleLink;
}


//
// Check for disk space problems
// Returns false if low disk space, and dialog is being shown
//
bool CCore::CheckDiskSpace( uint uiResourcesPathMinMB, uint uiDataPathMinMB )
{
    SString strDriveWithNoSpace = GetDriveNameWithNotEnoughSpace( uiResourcesPathMinMB, uiDataPathMinMB );
    if ( !strDriveWithNoSpace.empty() )
    {
        SString strMessage( _("MTA:SA cannot continue because drive %s does not have enough space."), *strDriveWithNoSpace );
        SString strTroubleLink( SString( "low-disk-space&drive=%s", *strDriveWithNoSpace.Left( 1 ) ) );
        g_pCore->ShowErrorMessageBox ( _("Fatal error")+_E("CC43"), strMessage, strTroubleLink );
        return false;
    }
    return true;
}


HWND CCore::GetHookedWindow ( void )
{
    return CMessageLoopHook::GetSingleton ().GetHookedWindowHandle ();
}


void CCore::HideMainMenu ( void )
{
    m_pLocalGUI->GetMainMenu ()->SetVisible ( false );
}

void CCore::HideQuickConnect ( void )
{
    m_pLocalGUI->GetMainMenu ()->GetQuickConnectWindow()->SetVisible( false );
}

void CCore::ShowServerInfo ( unsigned int WindowType )
{
    RemoveMessageBox ();
    CServerInfo::GetSingletonPtr()->Show( (eWindowType)WindowType );
}

void CCore::ApplyHooks ( )
{ 
    WriteDebugEvent ( "CCore::ApplyHooks" );

    // Create our hooks.
    m_pDirectInputHookManager->ApplyHook ( );
    //m_pDirect3DHookManager->ApplyHook ( );
    //m_pFileSystemHook->ApplyHook ( );
    m_pSetCursorPosHook->ApplyHook ( );

    // Redirect basic files.
    //m_pFileSystemHook->RedirectFile ( "main.scm", "../../mta/gtafiles/main.scm" );
}

bool UsingAltD3DSetup()
{
    static bool bAltStartup = GetApplicationSettingInt( "nvhacks", "optimus-alt-startup" ) ? true : false;
    return bAltStartup;
}

void CCore::ApplyHooks2 ( )
{ 
    WriteDebugEvent ( "CCore::ApplyHooks2" );
    // Try this one a little later
    if ( !UsingAltD3DSetup() )
        m_pDirect3DHookManager->ApplyHook ( );
    else
    {
        // Done a little later to get past the loading time required to decrypt the gta 
        // executable into memory...
        if ( !CCore::GetSingleton ( ).AreModulesLoaded ( ) )
        {
            CCore::GetSingleton ( ).SetModulesLoaded ( true );
            CCore::GetSingleton ( ).CreateNetwork ( );
            CCore::GetSingleton ( ).CreateGame ( );
            CCore::GetSingleton ( ).CreateMultiplayer ( );
            CCore::GetSingleton ( ).CreateXML ( );
            CCore::GetSingleton ( ).CreateGUI ( );
        }
    }
}

void CCore::ApplyHooks3( bool bEnable )
{
    if ( bEnable )
        CDirect3DHook9::GetSingletonPtr()->ApplyHook();
    else
        CDirect3DHook9::GetSingletonPtr()->RemoveHook();
}


void CCore::SetCenterCursor ( bool bEnabled )
{
    if ( bEnabled )
        m_pSetCursorPosHook->EnableSetCursorPos ();
    else
        m_pSetCursorPosHook->DisableSetCursorPos ();
}


////////////////////////////////////////////////////////////////////////
//
// LoadModule
//
// Attempt to load a module. Returns if successful.
// On failure, displays message box and terminates the current process.
//
////////////////////////////////////////////////////////////////////////
void LoadModule ( CModuleLoader& m_Loader, const SString& strName, const SString& strModuleName )
{
    WriteDebugEvent ( "Loading " + strName.ToLower () );

    // Ensure DllDirectory has not been changed
    SString strDllDirectory = GetSystemDllDirectory();
    if ( CalcMTASAPath ( "mta" ).CompareI ( strDllDirectory ) == false )
    {
        AddReportLog ( 3119, SString ( "DllDirectory wrong:  DllDirectory:'%s'  Path:'%s'", *strDllDirectory, *CalcMTASAPath ( "mta" ) ) );
        SetDllDirectory( CalcMTASAPath ( "mta" ) );
    }

    // Save current directory (shouldn't change anyway)
    SString strSavedCwd = GetSystemCurrentDirectory();

    // Load approrpiate compilation-specific library.
#ifdef MTA_DEBUG
    SString strModuleFileName = strModuleName + "_d.dll";
#else
    SString strModuleFileName = strModuleName + ".dll";
#endif
    m_Loader.LoadModule ( CalcMTASAPath ( PathJoin ( "mta", strModuleFileName ) ) );

    if ( m_Loader.IsOk () == false )
    {
        SString strMessage( "Error loading %s module! (%s)", *strName.ToLower (), *m_Loader.GetLastErrorMessage () );
        BrowseToSolution ( strModuleName + "-not-loadable", ASK_GO_ONLINE | TERMINATE_PROCESS, strMessage );
    }
    // Restore current directory
    SetCurrentDirectory ( strSavedCwd );

    WriteDebugEvent ( strName + " loaded." );
}


////////////////////////////////////////////////////////////////////////
//
// InitModule
//
// Attempt to initialize a loaded module. Returns if successful.
// On failure, displays message box and terminates the current process.
//
////////////////////////////////////////////////////////////////////////
template < class T, class U >
T* InitModule ( CModuleLoader& m_Loader, const SString& strName, const SString& strInitializer, U* pObj )
{
    // Save current directory (shouldn't change anyway)
    SString strSavedCwd = GetSystemCurrentDirectory();

    // Get initializer function from DLL.
    typedef T* (*PFNINITIALIZER) ( U* );
    PFNINITIALIZER pfnInit = static_cast < PFNINITIALIZER > ( m_Loader.GetFunctionPointer ( strInitializer ) );

    if ( pfnInit == NULL )
    {
        MessageBoxUTF8 ( 0, SString(_("%s module is incorrect!"),*strName), "Error"+_E("CC40"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST  );
        TerminateProcess ( GetCurrentProcess (), 1 );
    }

    // If we have a valid initializer, call it.
    T* pResult = pfnInit ( pObj );

    // Restore current directory
    SetCurrentDirectory ( strSavedCwd );

    WriteDebugEvent ( strName + " initialized." );
    return pResult;
}


////////////////////////////////////////////////////////////////////////
//
// CreateModule
//
// Attempt to load and initialize a module. Returns if successful.
// On failure, displays message box and terminates the current process.
//
////////////////////////////////////////////////////////////////////////
template < class T, class U >
T* CreateModule ( CModuleLoader& m_Loader, const SString& strName, const SString& strModuleName, const SString& strInitializer, U* pObj )
{
    LoadModule ( m_Loader, strName, strModuleName );
    return InitModule < T > ( m_Loader, strName, strInitializer, pObj );
}


void CCore::CreateGame ( )
{
    m_pGame = CreateModule < CGame > ( m_GameModule, "Game", "game_sa", "GetGameInterface", this );
    if ( m_pGame->GetGameVersion () >= VERSION_11 )
    {
        BrowseToSolution ( "downgrade", TERMINATE_PROCESS, "Only GTA:SA version 1.0 is supported!\n\nYou are now being redirected to a page where you can patch your version." );
    }
}


void CCore::CreateMultiplayer ( )
{
    m_pMultiplayer = CreateModule < CMultiplayer > ( m_MultiplayerModule, "Multiplayer", "multiplayer_sa", "InitMultiplayerInterface", this );
    if ( m_pMultiplayer )
        m_pMultiplayer->SetIdleHandler ( CCore::StaticIdleHandler );
}


void CCore::DeinitGUI ( void )
{

}


void CCore::InitGUI ( IDirect3DDevice9* pDevice )
{
    m_pGUI = InitModule < CGUI > ( m_GUIModule, "GUI", "InitGUIInterface", pDevice );

    // and set the screenshot path to this default library (screenshots shouldnt really be made outside mods)
    std::string strScreenShotPath = CalcMTASAPath ( "screenshots" );
    CVARS_SET ( "screenshot_path", strScreenShotPath );
    CScreenShot::SetPath ( strScreenShotPath.c_str() );
}


void CCore::CreateGUI ( void )
{
    LoadModule ( m_GUIModule, "GUI", "cgui" );
}

void CCore::DestroyGUI ( )
{
    WriteDebugEvent ( "CCore::DestroyGUI" );
    if ( m_pGUI )
    {
        m_pGUI = NULL;
    }
    m_GUIModule.UnloadModule ();
}


void CCore::CreateNetwork ( )
{
    m_pNet = CreateModule < CNet > ( m_NetModule, "Network", "netc", "InitNetInterface", this );

    // Network module compatibility check
    typedef unsigned long (*PFNCHECKCOMPATIBILITY) ( unsigned long, unsigned long* );
    PFNCHECKCOMPATIBILITY pfnCheckCompatibility = static_cast< PFNCHECKCOMPATIBILITY > ( m_NetModule.GetFunctionPointer ( "CheckCompatibility" ) );
    if ( !pfnCheckCompatibility || !pfnCheckCompatibility ( MTA_DM_CLIENT_NET_MODULE_VERSION, NULL ) )
    {
        // net.dll doesn't like our version number
        ulong ulNetModuleVersion = 0;
        pfnCheckCompatibility ( 1, &ulNetModuleVersion );
        SString strMessage( "Network module not compatible! (Expected 0x%x, got 0x%x)", MTA_DM_CLIENT_NET_MODULE_VERSION, ulNetModuleVersion );
        BrowseToSolution ( "netc-not-compatible", ASK_GO_ONLINE | TERMINATE_PROCESS, strMessage );
    }

    // Set mta version for report log here
    SetApplicationSetting ( "mta-version-ext", SString ( "%d.%d.%d-%d.%05d.%d.%03d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTASA_VERSION_TYPE
                                ,MTASA_VERSION_BUILD
                                ,m_pNet->GetNetRev ()
                                ,m_pNet->GetNetRel ()
                                ) );
    char szSerial [ 64 ];
    m_pNet->GetSerial ( szSerial, sizeof ( szSerial ) );
    SetApplicationSetting ( "serial", szSerial );
}


void CCore::CreateXML ( )
{
    if ( !m_pXML )
        m_pXML = CreateModule < CXML > ( m_XMLModule, "XML", "xmll", "InitXMLInterface", *CalcMTASAPath ( "MTA" ) );

    if ( !m_pConfigFile )
    {
        // Load config XML file
        m_pConfigFile = m_pXML->CreateXML ( CalcMTASAPath ( MTA_CONFIG_PATH ) );
        if ( !m_pConfigFile ) {
            assert ( false );
            return;
        }

        m_pConfigFile->Parse ();
    }

    // Load the keybinds (loads defaults if the subnode doesn't exist)
    if ( m_pKeyBinds )
    {
        m_pKeyBinds->LoadFromXML ( GetConfig ()->FindSubNode ( CONFIG_NODE_KEYBINDS ) );
        m_pKeyBinds->LoadDefaultCommands( false );
    }

    // Load XML-dependant subsystems
    m_ClientVariables.Load ( );
}


void CCore::DestroyGame ( )
{
    WriteDebugEvent ( "CCore::DestroyGame" );

    if ( m_pGame )
    {
        m_pGame->Terminate ();
        m_pGame = NULL;
    }

    m_GameModule.UnloadModule();

}


void CCore::DestroyMultiplayer ( )
{
    WriteDebugEvent ( "CCore::DestroyMultiplayer" );

    if ( m_pMultiplayer )
    {
        m_pMultiplayer = NULL;
    }

    m_MultiplayerModule.UnloadModule();
}


void CCore::DestroyXML ( )
{
    WriteDebugEvent ( "CCore::DestroyXML" );

    // Save and unload configuration
    if ( m_pConfigFile ) {
        SaveConfig ();
        delete m_pConfigFile;
    }

    if ( m_pXML )
    {
        m_pXML = NULL;
    }

    m_XMLModule.UnloadModule();
}


void CCore::DestroyNetwork ( )
{
    WriteDebugEvent ( "CCore::DestroyNetwork" );

    if ( m_pNet )
    {
        m_pNet->Shutdown();
        m_pNet = NULL;
    }

    m_NetModule.UnloadModule();
}


void CCore::UpdateIsWindowMinimized ( void )
{
    m_bIsWindowMinimized = IsIconic ( GetHookedWindow () ) ? true : false;
    // Update CPU saver for when minimized and not connected
    g_pCore->GetMultiplayer ()->SetIsMinimizedAndNotConnected ( m_bIsWindowMinimized && !IsConnected () );
    g_pCore->GetMultiplayer ()->SetMirrorsEnabled ( !m_bIsWindowMinimized );

    // Enable timer if not connected at least once
    bool bEnableTimer = !m_bGettingIdleCallsFromMultiplayer;
    if ( m_bWindowsTimerEnabled != bEnableTimer )
    {
        m_bWindowsTimerEnabled = bEnableTimer;
        if ( bEnableTimer )
            SetTimer( GetHookedWindow(), IDT_TIMER1, 50, (TIMERPROC) NULL );
        else
            KillTimer( GetHookedWindow(), IDT_TIMER1 );
    }
}


bool CCore::IsWindowMinimized ( void )
{
    return m_bIsWindowMinimized;
}


void CCore::DoPreFramePulse ( )
{
    TIMING_CHECKPOINT( "+CorePreFrame" );

    m_pKeyBinds->DoPreFramePulse ();

    // Notify the mod manager
    m_pModManager->DoPulsePreFrame ();  

    m_pLocalGUI->DoPulse ();
    TIMING_CHECKPOINT( "-CorePreFrame" );
}


void CCore::DoPostFramePulse ( )
{
    TIMING_CHECKPOINT( "+CorePostFrame1" );
    if ( m_bQuitOnPulse )
        Quit ();

    if ( m_bDestroyMessageBox )
    {
        RemoveMessageBox ();
        m_bDestroyMessageBox = false;
    }

    static bool bFirstPulse = true;
    if ( bFirstPulse )
    {
        bFirstPulse = false;

        // Validate CVARS
        CClientVariables::GetSingleton().ValidateValues ();

        // Apply all settings
        ApplyConsoleSettings ();
        ApplyGameSettings ();

        m_pGUI->SelectInputHandlers( INPUT_CORE );

        m_Community.Initialize ();
    }

    if ( m_pGame->GetSystemState () == 5 ) // GS_INIT_ONCE
    {
        WatchDogCompletedSection ( "L2" );      // gta_sa.set seems ok
        WatchDogCompletedSection ( "L3" );      // No hang on startup
    }

    // This is the first frame in the menu?
    if ( m_pGame->GetSystemState () == 7 ) // GS_FRONTEND
    {
        // Wait 250 frames more than the time it took to get status 7 (fade-out time)
        static short WaitForMenu = 0;

        // Do crash dump encryption while the credit screen is displayed
        if ( WaitForMenu == 0 )
            HandleCrashDumpEncryption();

        // Cope with early finish
        if ( m_pGame->HasCreditScreenFadedOut () )
            WaitForMenu = 250;

        if ( WaitForMenu >= 250 )
        {
            if ( m_bFirstFrame )
            {
                m_bFirstFrame = false;

                // Disable vsync while it's all dark
                m_pGame->DisableVSync ();

                // Parse the command line
                // Does it begin with mtasa://?
                if ( m_szCommandLineArgs && strnicmp ( m_szCommandLineArgs, "mtasa://", 8 ) == 0 )
                {
                    SString strArguments = GetConnectCommandFromURI ( m_szCommandLineArgs );
                    // Run the connect command
                    if ( strArguments.length () > 0 && !m_pCommands->Execute ( strArguments ) )
                    {
                        ShowMessageBox ( _("Error")+_E("CC41"), _("Error executing URL"), MB_BUTTON_OK | MB_ICON_ERROR );
                    }
                }
                else
                {
                    // We want to load a mod?
                    const char* szOptionValue;
                    if ( szOptionValue = GetCommandLineOption( "l" ) )
                    {
                        // Try to load the mod
                        if ( !m_pModManager->Load ( szOptionValue, m_szCommandLineArgs ) )
                        {
                            SString strTemp ( _("Error running mod specified in command line ('%s')"), szOptionValue );
                            ShowMessageBox ( _("Error")+_E("CC42"), strTemp, MB_BUTTON_OK | MB_ICON_ERROR ); // Command line Mod load failed
                        }
                    }
                    // We want to connect to a server?
                    else if ( szOptionValue = GetCommandLineOption ( "c" ) )
                    {
                        CCommandFuncs::Connect ( szOptionValue );
                    }
                }
            }
        }
        else
        {
            WaitForMenu++;
        }

        if ( m_bWaitToSetNick && GetLocalGUI()->GetMainMenu()->IsVisible() && !GetLocalGUI()->GetMainMenu()->IsFading() )
        {
            if ( m_uiNewNickWaitFrames > 75 )
            {
                // Request a new nickname if we're waiting for one
                GetLocalGUI()->GetMainMenu()->GetSettingsWindow()->RequestNewNickname();
                m_bWaitToSetNick = false;
            }
            else
                m_uiNewNickWaitFrames++;
        }
    }

    if ( !IsFocused() && m_bLastFocused )
    {
        // Fix for #4948
        m_pKeyBinds->CallAllGTAControlBinds ( CONTROL_BOTH, false );
        m_bLastFocused = false;
    }
    else if ( IsFocused() && !m_bLastFocused )
    {
        m_bLastFocused = true;
    }

    GetJoystickManager ()->DoPulse ();      // Note: This may indirectly call CMessageLoopHook::ProcessMessage
    m_pKeyBinds->DoPostFramePulse ();

    // Notify the mod manager and the connect manager
    TIMING_CHECKPOINT( "-CorePostFrame1" );
    m_pModManager->DoPulsePostFrame ();
    TIMING_CHECKPOINT( "+CorePostFrame2" );
    GetMemStats ()->Draw ();
    GetGraphStats ()->Draw();
    m_pConnectManager->DoPulse ();

    m_Community.DoPulse ();

    //XFire polling
    if ( IsConnected() )
    {
        time_t ttime;
        ttime = time ( NULL );
        if ( ttime >= m_tXfireUpdate + XFIRE_UPDATE_RATE )
        {
            if ( m_pCurrentServer->IsSocketClosed() )
            {
                //Init our socket
                m_pCurrentServer->Init();
            }
            //Get our xfire query reply
            SString strReply = UpdateXfire( );
            //If we Parsed or if the reply failed wait another XFIRE_UPDATE_RATE until trying again
            if ( strReply == "ParsedQuery" || strReply == "NoReply" ) 
            {
                m_tXfireUpdate = time ( NULL );
                //Close the socket
                m_pCurrentServer->SocketClose();
            }
        }
    }
    //Set our update time to zero to ensure that the first xfire update happens instantly when joining
    else
    {
        XfireSetCustomGameData ( 0, NULL, NULL );
        if ( m_tXfireUpdate != 0 )
            m_tXfireUpdate = 0;
    }
    TIMING_CHECKPOINT( "-CorePostFrame2" );
}


// Called after MOD is unloaded
void CCore::OnModUnload ( )
{
    // reattach the global event
    m_pGUI->SelectInputHandlers( INPUT_CORE );
    // remove unused events
    m_pGUI->ClearInputHandlers( INPUT_MOD );

    // Ensure all these have been removed
    m_pKeyBinds->RemoveAllFunctions ();
    m_pKeyBinds->RemoveAllControlFunctions ();

    // Reset client script frame rate limit
    m_uiClientScriptFrameRateLimit = 0;
}


void CCore::RegisterCommands ( )
{
    //m_pCommands->Add ( "e", CCommandFuncs::Editor );
    //m_pCommands->Add ( "clear", CCommandFuncs::Clear );
    m_pCommands->Add ( "help",              _("this help screen"),                 CCommandFuncs::Help );
    m_pCommands->Add ( "exit",              _("exits the application"),            CCommandFuncs::Exit );
    m_pCommands->Add ( "quit",              _("exits the application"),            CCommandFuncs::Exit );
    m_pCommands->Add ( "ver",               _("shows the version"),                CCommandFuncs::Ver );
    m_pCommands->Add ( "time",              _("shows the time"),                   CCommandFuncs::Time );
    m_pCommands->Add ( "showhud",           _("shows the hud"),                    CCommandFuncs::HUD );
    m_pCommands->Add ( "binds",             _("shows all the binds"),              CCommandFuncs::Binds );
    m_pCommands->Add ( "serial",            _("shows your serial"),                CCommandFuncs::Serial );

#if 0
    m_pCommands->Add ( "vid",               "changes the video settings (id)",  CCommandFuncs::Vid );
    m_pCommands->Add ( "window",            "enter/leave windowed mode",        CCommandFuncs::Window );
    m_pCommands->Add ( "load",              "loads a mod (name args)",          CCommandFuncs::Load );
    m_pCommands->Add ( "unload",            "unloads a mod (name)",             CCommandFuncs::Unload );
#endif

    m_pCommands->Add ( "connect",           _("connects to a server (host port nick pass)"),   CCommandFuncs::Connect );
    m_pCommands->Add ( "reconnect",         _("connects to a previous server"),    CCommandFuncs::Reconnect );
    m_pCommands->Add ( "bind",              _("binds a key (key control)"),        CCommandFuncs::Bind );
    m_pCommands->Add ( "unbind",            _("unbinds a key (key)"),              CCommandFuncs::Unbind );
    m_pCommands->Add ( "copygtacontrols",   _("copies the default gta controls"),  CCommandFuncs::CopyGTAControls );
    m_pCommands->Add ( "screenshot",        _("outputs a screenshot"),             CCommandFuncs::ScreenShot );
    m_pCommands->Add ( "saveconfig",        _("immediately saves the config"),     CCommandFuncs::SaveConfig );

    m_pCommands->Add ( "cleardebug",        _("clears the debug view"),            CCommandFuncs::DebugClear );
    m_pCommands->Add ( "chatscrollup",      _("scrolls the chatbox upwards"),      CCommandFuncs::ChatScrollUp );
    m_pCommands->Add ( "chatscrolldown",    _("scrolls the chatbox downwards"),    CCommandFuncs::ChatScrollDown );
    m_pCommands->Add ( "debugscrollup",     _("scrolls the debug view upwards"),   CCommandFuncs::DebugScrollUp );
    m_pCommands->Add ( "debugscrolldown",   _("scrolls the debug view downwards"), CCommandFuncs::DebugScrollDown );

    m_pCommands->Add ( "test",              "",                                 CCommandFuncs::Test );
    m_pCommands->Add ( "showmemstat",       _("shows the memory statistics"),      CCommandFuncs::ShowMemStat );
    m_pCommands->Add ( "showframegraph",    _("shows the frame timing graph"),     CCommandFuncs::ShowFrameGraph );
    m_pCommands->Add ( "jinglebells",       "",                                    CCommandFuncs::JingleBells );

#if defined(MTA_DEBUG) || defined(MTA_BETA)
    m_pCommands->Add ( "fakelag",           "",                                 CCommandFuncs::FakeLag );
#endif
}


void CCore::SwitchRenderWindow ( HWND hWnd, HWND hWndInput )
{
    assert ( 0 );
#if 0
    // Make GTA windowed
    m_pGame->GetSettings()->SetCurrentVideoMode(0);

    // Get the destination window rectangle
    RECT rect;
    GetWindowRect ( hWnd, &rect );

    // Size the GTA window size to the same size as the destination window rectangle
    HWND hDeviceWindow = CDirect3DData::GetSingleton ().GetDeviceWindow ();
    MoveWindow ( hDeviceWindow,
                 0,
                 0,
                 rect.right - rect.left,
                 rect.bottom - rect.top,
                 TRUE );

    // Turn the GTA window into a child window of our static render container window
    SetParent ( hDeviceWindow, hWnd );
    SetWindowLong ( hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_CHILD );
#endif
}


bool CCore::IsValidNick ( const char* szNick )
{
    // Grab the size of the nick. Check that it's within the player 
    size_t sizeNick = strlen(szNick);
    if (sizeNick < MIN_PLAYER_NICK_LENGTH || sizeNick > MAX_PLAYER_NICK_LENGTH)
    {
        return false;
    }

    // Check that each character is valid (visible characters exluding space)
    unsigned char ucTemp;
    for (size_t i = 0; i < sizeNick; i++)
    {
        ucTemp = szNick[i];
        if (ucTemp < 33 || ucTemp > 126)
        {
            return false;
        }
    }

    // Nickname is valid, return true
    return true;
   
}


void CCore::Quit ( bool bInstantly )
{
    if ( bInstantly )
    {
        // Show that we are quiting (for the crash dump filename)
        SetApplicationSettingInt ( "last-server-ip", 1 );

        WatchDogBeginSection( "Q0" );   // Allow loader to detect freeze on exit

        // Destroy the client
        CModManager::GetSingleton ().Unload ();

        // Destroy ourself
        delete CCore::GetSingletonPtr ();

        WatchDogCompletedSection( "Q0" );

        // Use TerminateProcess for now as exiting the normal way crashes
        TerminateProcess ( GetCurrentProcess (), 0 );
        //PostQuitMessage ( 0 );
    }
    else
    {
        m_bQuitOnPulse = true;
    }
}


bool CCore::WasLaunchedWithConnectURI ( void )
{
    if ( m_szCommandLineArgs && strnicmp ( m_szCommandLineArgs, "mtasa://", 8 ) == 0 )
        return true;
    return false;
}


void CCore::ParseCommandLine ( std::map < std::string, std::string > & options, const char*& szArgs, const char** pszNoValOptions )
{
    std::set < std::string > noValOptions;
    if ( pszNoValOptions )
    {
        while ( *pszNoValOptions )
        {
            noValOptions.insert ( *pszNoValOptions );
            pszNoValOptions++;
        }
    }

    const char* szCmdLine = GetCommandLine ();
    char szCmdLineCopy[512];
    STRNCPY ( szCmdLineCopy, szCmdLine, sizeof(szCmdLineCopy) );
    
    char* pCmdLineEnd = szCmdLineCopy + strlen ( szCmdLineCopy );
    char* pStart = szCmdLineCopy;
    char* pEnd = pStart;
    bool bInQuoted = false;
    std::string strKey;
    szArgs = NULL;

    while ( pEnd != pCmdLineEnd )
    {
        pEnd = strchr ( pEnd + 1, ' ' );
        if ( !pEnd )
            pEnd = pCmdLineEnd;
        if ( bInQuoted && *(pEnd - 1) == '"' )
            bInQuoted = false;
        else if ( *pStart == '"' )
            bInQuoted = true;

        if ( !bInQuoted )
        {
            *pEnd = 0;
            if ( strKey.empty () )
            {
                if ( *pStart == '-' )
                {
                    strKey = pStart + 1;
                    if ( noValOptions.find ( strKey ) != noValOptions.end () )
                    {
                        options [ strKey ] = "";
                        strKey.clear ();
                    }
                }
                else
                {
                    szArgs = pStart - szCmdLineCopy + szCmdLine;
                    break;
                }
            }
            else
            {
                if ( *pStart == '-' )
                {
                    options [ strKey ] = "";
                    strKey = pStart + 1;
                }
                else
                {
                    if ( *pStart == '"' )
                        pStart++;
                    if ( *(pEnd - 1) == '"' )
                        *(pEnd - 1) = 0;
                    options [ strKey ] = pStart;
                    strKey.clear ();
                }
            }
            pStart = pEnd;
            while ( pStart != pCmdLineEnd && *(++pStart) == ' ' );
            pEnd = pStart;
        }
    }
}

const char* CCore::GetCommandLineOption ( const char* szOption )
{
    std::map < std::string, std::string >::iterator it = m_CommandLineOptions.find ( szOption );
    if ( it != m_CommandLineOptions.end () )
        return it->second.c_str ();
    else
        return NULL;
}

SString CCore::GetConnectCommandFromURI ( const char* szURI )
{
    unsigned short usPort;
    std::string strHost, strNick, strPassword;
    GetConnectParametersFromURI ( szURI, strHost, usPort, strNick, strPassword );

    // Generate a string with the arguments to send to the mod IF we got a host
    SString strDest;
    if ( strHost.size() > 0 )
    {
        if ( strPassword.size() > 0 )
            strDest.Format ( "connect %s %u %s %s", strHost.c_str (), usPort, strNick.c_str (), strPassword.c_str () );
        else
            strDest.Format ( "connect %s %u %s", strHost.c_str (), usPort, strNick.c_str () );
    }

    return strDest;
}

void CCore::GetConnectParametersFromURI ( const char* szURI, std::string &strHost, unsigned short &usPort, std::string &strNick, std::string &strPassword )
{
    // Grab the length of the string
    size_t sizeURI = strlen ( szURI );
    
    // Parse it right to left
    char szLeft [256];
    szLeft [255] = 0;
    char* szLeftIter = szLeft + 255;

    char szRight [256];
    szRight [255] = 0;
    char* szRightIter = szRight + 255;

    const char* szIterator = szURI + sizeURI;
    bool bHitAt = false;

    for ( ; szIterator >= szURI + 8; szIterator-- )
    {
        if ( !bHitAt && *szIterator == '@' )
        {
            bHitAt = true;
        }
        else
        {
            if ( bHitAt )
            {
                if ( szLeftIter > szLeft )
                {
                    *(--szLeftIter) = *szIterator;
                }
            }
            else
            {
                if ( szRightIter > szRight )
                {
                    *(--szRightIter) = *szIterator;
                }
            }
        }
    }

    // Parse the host/port
    char szHost [64];
    char szPort [12];
    char* szHostIter = szHost;
    char* szPortIter = szPort;
    memset ( szHost, 0, sizeof(szHost) );
    memset ( szPort, 0, sizeof(szPort) );

    bool bIsInPort = false;
    size_t sizeRight = strlen ( szRightIter );
    for ( size_t i = 0; i < sizeRight; i++ )
    {
        if ( !bIsInPort && szRightIter [i] == ':' )
        {
            bIsInPort = true;
        }
        else
        {
            if ( bIsInPort )
            {
                if ( szPortIter < szPort + 11 )
                {
                    *(szPortIter++) = szRightIter [i];
                }
            }
            else
            {
                if ( szHostIter < szHost + 63 )
                {
                    *(szHostIter++) = szRightIter [i];
                }
            }
        }

    }

    // Parse the nickname / password
    char szNickname [64];
    char szPassword [64];
    char* szNicknameIter = szNickname;
    char* szPasswordIter = szPassword;
    memset ( szNickname, 0, sizeof(szNickname) );
    memset ( szPassword, 0, sizeof(szPassword) );

    bool bIsInPassword = false;
    size_t sizeLeft = strlen ( szLeftIter );
    for ( size_t i = 0; i < sizeLeft; i++ )
    {
        if ( !bIsInPassword && szLeftIter [i] == ':' )
        {
            bIsInPassword = true;
        }
        else
        {
            if ( bIsInPassword )
            {
                if ( szPasswordIter < szPassword + 63 )
                {
                    *(szPasswordIter++) = szLeftIter [i];
                }
            }
            else
            {
                if ( szNicknameIter < szNickname + 63 )
                {
                    *(szNicknameIter++) = szLeftIter [i];
                }
            }
        }

    }

    // If we got any port, convert it to an integral type
    usPort = 22003;
    if ( strlen ( szPort ) > 0 )
    {
        usPort = static_cast < unsigned short > ( atoi ( szPort ) );
    }

    // Grab the nickname
    if ( strlen ( szNickname ) > 0 )
    {
        strNick = szNickname;
    }
    else
    {
        CVARS_GET ( "nick", strNick );
    }
    strHost = szHost;
    strPassword = szPassword;
}


void CCore::UpdateRecentlyPlayed()
{
    //Get the current host and port
    unsigned int uiPort;
    std::string strHost;
    CVARS_GET ( "host", strHost );
    CVARS_GET ( "port", uiPort );
    // Save the connection details into the recently played servers list
    in_addr Address;
    if ( CServerListItem::Parse ( strHost.c_str(), Address ) )
    {
        CServerBrowser* pServerBrowser = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetServerBrowser ();
        CServerList* pRecentList = pServerBrowser->GetRecentList ();
        pRecentList->Remove ( Address, uiPort );
        pRecentList->AddUnique ( Address, uiPort, true );
       
        pServerBrowser->SaveRecentlyPlayedList();
        if ( !m_pConnectManager->m_strLastPassword.empty() )
            pServerBrowser->SetServerPassword ( strHost + ":" + SString("%u",uiPort), m_pConnectManager->m_strLastPassword );

    }
    //Save our configuration file
    CCore::GetSingleton ().SaveConfig ();
}

void CCore::SetCurrentServer( in_addr Addr, unsigned short usGamePort )
{
    //Set the current server info so we can query it with ASE for xfire
    m_pCurrentServer->ChangeAddress( Addr, usGamePort );
}

SString CCore::UpdateXfire( void )
{
    //Check if a current server exists
    if ( m_pCurrentServer ) 
    {
        //Get the result from the Pulse method
        std::string strResult = m_pCurrentServer->Pulse();
        //Have we parsed the query this function call?
        if ( strResult == "ParsedQuery" )
        {
            //Get our Nick from CVARS
            std::string strNick;
            CVARS_GET ( "nick", strNick );
            //Format a player count
            SString strPlayerCount("%i / %i", m_pCurrentServer->nPlayers, m_pCurrentServer->nMaxPlayers);
            // Set as our custom date
            SetXfireData( m_pCurrentServer->strName, m_pCurrentServer->strVersion, m_pCurrentServer->bPassworded, m_pCurrentServer->strGameMode, m_pCurrentServer->strMap, strNick, strPlayerCount );
        }
        //Return the result
        return strResult;
    }
    return "";
}

void CCore::SetXfireData ( std::string strServerName, std::string strVersion, bool bPassworded, std::string strGamemode, std::string strMap, std::string strPlayerName, std::string strPlayerCount )
{
    if ( XfireIsLoaded () )
    {
        //Set our "custom data"
        const char *szKey[7], *szValue[7];
        szKey[0] = "Server Name";
        szValue[0] = strServerName.c_str();

        szKey[1] = "Server Version";
        szValue[1] = strVersion.c_str();

        szKey[2] = "Passworded";
        szValue[2] = bPassworded ? "Yes" : "No";

        szKey[3] = "Gamemode";
        szValue[3] = strGamemode.c_str();

        szKey[4] = "Map";
        szValue[4] = strMap.c_str();

        szKey[5] = "Player Name";
        szValue[5] = strPlayerName.c_str();

        szKey[6] = "Player Count";
        szValue[6] = strPlayerCount.c_str();
        
        XfireSetCustomGameData ( 7, szKey, szValue );
    }
}


//
// Recalculate FPS limit to use
//
// Uses client rate from config
// Uses client rate from script
// Uses server rate from argument, or last time if not supplied
//
void CCore::RecalculateFrameRateLimit ( uint uiServerFrameRateLimit, bool bLogToConsole )
{
    // Save rate from server if valid
    if ( uiServerFrameRateLimit != -1 )
        m_uiServerFrameRateLimit = uiServerFrameRateLimit;

    // Start with value set by the server
    m_uiFrameRateLimit = m_uiServerFrameRateLimit;

    // Apply client config setting
    uint uiClientConfigRate;
    g_pCore->GetCVars ()->Get ( "fps_limit", uiClientConfigRate );
    // Lowest wins (Although zero is highest)
    if ( ( m_uiFrameRateLimit == 0 || uiClientConfigRate < m_uiFrameRateLimit ) && uiClientConfigRate > 0 )
        m_uiFrameRateLimit = uiClientConfigRate;

    // Apply client script setting
    uint uiClientScriptRate = m_uiClientScriptFrameRateLimit;
    // Lowest wins (Although zero is highest)
    if ( ( m_uiFrameRateLimit == 0 || uiClientScriptRate < m_uiFrameRateLimit ) && uiClientScriptRate > 0 )
        m_uiFrameRateLimit = uiClientScriptRate;

    // Print new limits to the console
    if ( bLogToConsole )
    {
        SString strStatus (  "Server FPS limit: %d", m_uiServerFrameRateLimit );
        if ( m_uiFrameRateLimit != m_uiServerFrameRateLimit )
            strStatus += SString (  " (Using %d)", m_uiFrameRateLimit );
        CCore::GetSingleton ().GetConsole ()->Print( strStatus );
    }
}


//
// Change client rate as set by script
//
void CCore::SetClientScriptFrameRateLimit ( uint uiClientScriptFrameRateLimit )
{
    m_uiClientScriptFrameRateLimit = uiClientScriptFrameRateLimit;
    RecalculateFrameRateLimit( -1, false );
}


//
// Make sure the frame rate limit has been applied since the last call
//
void CCore::EnsureFrameRateLimitApplied ( void )
{
    if ( !m_bDoneFrameRateLimit )
    {
        ApplyFrameRateLimit ();
    }
    m_bDoneFrameRateLimit = false;
}


//
// Do FPS limiting
//
// This is called once a frame even if minimized
//
void CCore::ApplyFrameRateLimit ( uint uiOverrideRate )
{
    TIMING_CHECKPOINT( "-CallIdle1" );
    ms_TimingCheckpoints.EndTimingCheckpoints ();

    // Frame rate limit stuff starts here
    m_bDoneFrameRateLimit = true;

    uint uiUseRate = uiOverrideRate != -1 ? uiOverrideRate : m_uiFrameRateLimit;

    TIMING_GRAPH("Limiter");

    if ( uiUseRate < 1 )
        return DoReliablePulse ();

    if ( m_DiagnosticDebug != EDiagnosticDebug::D3D_6732 )
         Sleep( 1 );    // Make frame rate smoother maybe

    // Calc required time in ms between frames
    const double dTargetTimeToUse = 1000.0 / uiUseRate;

    // Time now
    double dTimeMs = GetTickCount32 ();

    // Get delta time in ms since last frame
    double dTimeUsed = dTimeMs - m_dLastTimeMs;

    // Apply any over/underrun carried over from the previous frame
    dTimeUsed += m_dPrevOverrun;

    if ( dTimeUsed < dTargetTimeToUse )
    {
        // Have time spare - maybe eat some of that now
        double dSpare = dTargetTimeToUse - dTimeUsed;

        double dUseUpNow = dSpare - dTargetTimeToUse * 0.2f;
        if ( dUseUpNow >= 1 )
            Sleep( static_cast < DWORD > ( floor ( dUseUpNow ) ) );

        // Redo timing calcs
        dTimeMs = GetTickCount32 ();
        dTimeUsed = dTimeMs - m_dLastTimeMs;
        dTimeUsed += m_dPrevOverrun;
    }

    // Update over/underrun for next frame
    m_dPrevOverrun = dTimeUsed - dTargetTimeToUse;

    // Limit carry over
    m_dPrevOverrun = Clamp ( dTargetTimeToUse * -0.9f, m_dPrevOverrun, dTargetTimeToUse * 0.1f );

    m_dLastTimeMs = dTimeMs;

    DoReliablePulse ();

    TIMING_GRAPH("FrameEnd");
    TIMING_GRAPH("");
}


//
// DoReliablePulse
//
// This is called once a frame even if minimized
//
void CCore::DoReliablePulse ( void )
{
    ms_TimingCheckpoints.BeginTimingCheckpoints ();
    TIMING_CHECKPOINT( "+CallIdle2" );

    UpdateIsWindowMinimized ();

    // Non frame rate limit stuff
    if ( IsWindowMinimized () )
        m_iUnminimizeFrameCounter = 4;     // Tell script we have unminimized after a short delay

    UpdateModuleTickCount64 ();
}


//
// Debug timings
//
bool CCore::IsTimingCheckpoints ( void )
{
    return ms_TimingCheckpoints.IsTimingCheckpoints ();
}

void CCore::OnTimingCheckpoint ( const char* szTag )
{
    ms_TimingCheckpoints.OnTimingCheckpoint ( szTag );
}

void CCore::OnTimingDetail ( const char* szTag )
{
    ms_TimingCheckpoints.OnTimingDetail ( szTag );
}


//
// OnDeviceRestore
//
void CCore::OnDeviceRestore ( void )
{
    m_iUnminimizeFrameCounter = 4;     // Tell script we have restored after 4 frames to avoid double sends
    m_bDidRecreateRenderTargets = true;
}


//
// OnPreFxRender
//
void CCore::OnPreFxRender ( void )
{
    // Don't do nothing if nothing won't be drawn
    if ( !CGraphics::GetSingleton ().HasMaterialLine3DQueueItems () )
        return;

    CGraphics::GetSingleton ().EnteringMTARenderZone();

    CGraphics::GetSingleton ().DrawMaterialLine3DQueue ();

    CGraphics::GetSingleton ().LeavingMTARenderZone();
}


//
// OnPreHUDRender
//
void CCore::OnPreHUDRender ( void )
{
    IDirect3DDevice9* pDevice = CGraphics::GetSingleton ().GetDevice ();

    // Handle saving depth buffer
    CGraphics::GetSingleton ().GetRenderItemManager ()->SaveReadableDepthBuffer();

    CGraphics::GetSingleton ().EnteringMTARenderZone();

    // Maybe capture screen and other stuff
    CGraphics::GetSingleton ().GetRenderItemManager ()->DoPulse ();

    // Handle script stuffs
    if ( m_iUnminimizeFrameCounter && --m_iUnminimizeFrameCounter == 0 )
    {
        m_pModManager->DoPulsePreHUDRender ( true, m_bDidRecreateRenderTargets );
        m_bDidRecreateRenderTargets = false;
    }
    else
        m_pModManager->DoPulsePreHUDRender ( false, false );

    // Restore in case script forgets
    CGraphics::GetSingleton ().GetRenderItemManager ()->RestoreDefaultRenderTarget ();

    // Draw pre-GUI primitives
    CGraphics::GetSingleton ().DrawPreGUIQueue ();

    CGraphics::GetSingleton ().LeavingMTARenderZone();
}


//
// CCore::CalculateStreamingMemoryRange
//
// Streaming memory range based on system installed memory:
//
//     System RAM MB     min     max
//           512     =   64      96
//          1024     =   96     128
//          2048     =  128     256
//
// Also:
//   Max should be no more than 2 * installed video memory
//   Min should be no more than 1 * installed video memory
//   Max should be no less than 96MB
//   Gap between min and max should be no less than 32MB
//
void CCore::CalculateStreamingMemoryRange ( void )
{
    // Only need to do this once
    if ( m_fMinStreamingMemory != 0 )
        return;

    // Get system info
    int iSystemRamMB = static_cast < int > ( GetWMITotalPhysicalMemory () / 1024LL / 1024LL );
    int iVideoMemoryMB = g_pDeviceState->AdapterState.InstalledMemoryKB / 1024;

    // Calc min and max from lookup table
    SSamplePoint < float > minPoints[] = { {512, 64},  {1024, 96},   {2048, 128} };
    SSamplePoint < float > maxPoints[] = { {512, 96},  {1024, 128},  {2048, 256} };

    float fMinAmount = EvalSamplePosition < float > ( minPoints, NUMELMS ( minPoints ), iSystemRamMB );
    float fMaxAmount = EvalSamplePosition < float > ( maxPoints, NUMELMS ( maxPoints ), iSystemRamMB );

    // Apply cap dependant on video memory
    fMaxAmount = Min ( fMaxAmount, iVideoMemoryMB * 2.f );
    fMinAmount = Min ( fMinAmount, iVideoMemoryMB * 1.f );

    // Apply 96MB lower limit
    fMaxAmount = Max ( fMaxAmount, 96.f );

    // Apply gap limit
    fMinAmount = fMaxAmount - Max ( fMaxAmount - fMinAmount, 32.f );

    m_fMinStreamingMemory = fMinAmount;
    m_fMaxStreamingMemory = fMaxAmount;
}


//
// GetMinStreamingMemory
//
uint CCore::GetMinStreamingMemory ( void )
{
    CalculateStreamingMemoryRange ();

#ifdef MTA_DEBUG
    return 1;
#endif
    return m_fMinStreamingMemory;
}


//
// GetMaxStreamingMemory
//
uint CCore::GetMaxStreamingMemory ( void )
{
    CalculateStreamingMemoryRange ();
    return m_fMaxStreamingMemory;
}


//
// OnCrashAverted
// 
void CCore::OnCrashAverted ( uint uiId )
{
    CCrashDumpWriter::OnCrashAverted ( uiId );
}


//
// LogEvent
// 
void CCore::LogEvent ( uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId )
{
    if ( uiAddReportLogId )
        AddReportLog ( uiAddReportLogId, SString ( "%s - %s", szContext, szBody ) );

    if ( GetDebugIdEnabled ( uiDebugId ) )
    {
        CCrashDumpWriter::LogEvent ( szType, szContext, szBody );
        OutputDebugLine ( SString ( "[LogEvent] %d %s %s %s", uiDebugId, szType, szContext, szBody ) );
    }
}


//
// GetDebugIdEnabled
// 
bool CCore::GetDebugIdEnabled ( uint uiDebugId )
{
    static CFilterMap debugIdFilterMap ( GetVersionUpdater ()->GetDebugFilterString () );
    return ( uiDebugId == 0 ) || !debugIdFilterMap.IsFiltered ( uiDebugId );
}

EDiagnosticDebugType CCore::GetDiagnosticDebug ( void )
{
    return m_DiagnosticDebug;
}

void CCore::SetDiagnosticDebug ( EDiagnosticDebugType value )
{
    m_DiagnosticDebug = value;
}


CModelCacheManager* CCore::GetModelCacheManager ( void )
{
    if ( !m_pModelCacheManager )
        m_pModelCacheManager = NewModelCacheManager ();
    return m_pModelCacheManager;
}


void CCore::AddModelToPersistentCache ( ushort usModelId )
{
    return GetModelCacheManager ()->AddModelToPersistentCache ( usModelId );
}


void CCore::StaticIdleHandler ( void )
{
    g_pCore->IdleHandler ();
}


// Gets called every game loop, after GTA has been loaded for the first time
void CCore::IdleHandler ( void )
{
    m_bGettingIdleCallsFromMultiplayer = true;
    HandleIdlePulse();
}


// Gets called every 50ms, before GTA has been loaded for the first time
void CCore::WindowsTimerHandler ( void )
{
    if ( !m_bGettingIdleCallsFromMultiplayer )
        HandleIdlePulse();
}


// Always called, even if minimized
void CCore::HandleIdlePulse ( void )
{
    UpdateIsWindowMinimized();

    if ( IsWindowMinimized() )
    {
        DoPreFramePulse();
        DoPostFramePulse();
    }
    if ( m_pModManager->GetCurrentMod() )
        m_pModManager->GetCurrentMod()->IdleHandler();
}


//
// Handle encryption of Windows crash dump files
//
void CCore::HandleCrashDumpEncryption( void )
{
    const int iMaxFiles = 10;
    SString strDumpDirPath = CalcMTASAPath( "mta\\dumps" );
    SString strDumpDirPrivatePath = PathJoin( strDumpDirPath, "private" );
    SString strDumpDirPublicPath = PathJoin( strDumpDirPath, "public" );
    MakeSureDirExists( strDumpDirPrivatePath + "/" );
    MakeSureDirExists( strDumpDirPublicPath + "/" );

    SString strMessage = "Dump files in this directory are encrypted and copied to 'dumps\\public' during startup\n\n";
    FileSave( PathJoin( strDumpDirPrivatePath, "README.txt" ), strMessage );

    // Move old dumps to the private folder
    {
        std::vector < SString > legacyList = FindFiles( PathJoin( strDumpDirPath, "*.dmp" ), true, false );
        for ( uint i = 0 ; i < legacyList.size() ; i++ )
        {
            const SString& strFilename = legacyList[i];
            SString strSrcPathFilename = PathJoin( strDumpDirPath, strFilename );
            SString strDestPathFilename = PathJoin( strDumpDirPrivatePath, strFilename );
            FileRename( strSrcPathFilename, strDestPathFilename );
        }
    }

    // Limit number of files in the private folder
    {
        std::vector < SString > privateList = FindFiles( PathJoin( strDumpDirPrivatePath, "*.dmp" ), true, false, true );
        for ( int i = 0 ; i < (int)privateList.size() - iMaxFiles ; i++ )
            FileDelete( PathJoin( strDumpDirPrivatePath, privateList[i] ) );
    }

    // Copy and encrypt private files to public if they don't already exist
    {
        std::vector < SString > privateList = FindFiles( PathJoin( strDumpDirPrivatePath, "*.dmp" ), true, false );
        for ( uint i = 0 ; i < privateList.size() ; i++ )
        {
            const SString& strPrivateFilename = privateList[i];
            SString strPublicFilename = ExtractBeforeExtension( strPrivateFilename ) + ".rsa." + ExtractExtension( strPrivateFilename );
            SString strPrivatePathFilename = PathJoin( strDumpDirPrivatePath, strPrivateFilename );
            SString strPublicPathFilename = PathJoin( strDumpDirPublicPath, strPublicFilename );
            if ( !FileExists( strPublicPathFilename ) )
            {
               GetNetwork()->EncryptDumpfile( strPrivatePathFilename, strPublicPathFilename );
            }
        }
    }

    // Limit number of files in the public folder
    {
        std::vector < SString > publicList = FindFiles( PathJoin( strDumpDirPublicPath, "*.dmp" ), true, false, true );
        for ( int i = 0 ; i < (int)publicList.size() - iMaxFiles ; i++ )
            FileDelete( PathJoin( strDumpDirPublicPath, publicList[i] ) );
    }

    // And while we are here, limit number of items in core.log as well
    {
        SString strCoreLogPathFilename = CalcMTASAPath( "mta\\core.log" );
        SString strFileContents;
        FileLoad( strCoreLogPathFilename, strFileContents );

        SString strDelmiter = "** -- Unhandled exception -- **";
        std::vector < SString > parts;
        strFileContents.Split( strDelmiter, parts );

        if ( parts.size() > iMaxFiles )
        {
            strFileContents = strDelmiter + strFileContents.Join( strDelmiter, parts, parts.size() - iMaxFiles );
            FileSave( strCoreLogPathFilename, strFileContents );
        }
    }
}

//
// Flag to make sure stuff only gets done when everything is ready
//
void CCore::SetModulesLoaded( bool bLoaded )
{
    m_bModulesLoaded = bLoaded;
}

bool CCore::AreModulesLoaded( void )
{
    return m_bModulesLoaded;
}

//
// Handle dummy progress when game seems stalled
//
int ms_iDummyProgressTimerCounter = 0;

void CALLBACK TimerProc( void* lpParametar, BOOLEAN TimerOrWaitFired )
{
    ms_iDummyProgressTimerCounter++;
}

//
// Refresh progress output
//
void CCore::UpdateDummyProgress( int iValue, const char* szType )
{
    if ( iValue != -1 )
    {
        m_iDummyProgressValue = iValue;
        m_strDummyProgressType = szType;
    }

    if ( m_DummyProgressTimerHandle == NULL )
    {
        // Using this timer is quicker than checking tick count with every call to UpdateDummyProgress()
        ::CreateTimerQueueTimer( &m_DummyProgressTimerHandle, NULL, TimerProc, this, DUMMY_PROGRESS_ANIMATION_INTERVAL, DUMMY_PROGRESS_ANIMATION_INTERVAL, WT_EXECUTEINTIMERTHREAD );
    }

    if ( !ms_iDummyProgressTimerCounter )
        return;
    ms_iDummyProgressTimerCounter = 0;

    // Compose message with amount
    SString strMessage;
    if ( m_iDummyProgressValue )
        strMessage = SString( "%d%s", m_iDummyProgressValue, *m_strDummyProgressType );

    CGraphics::GetSingleton().SetProgressMessage( strMessage );
}

//
// Do SetCursorPos if allowed
//
void CCore::CallSetCursorPos( int X, int Y )
{
    if ( CCore::GetSingleton ().IsFocused () && !CLocalGUI::GetSingleton ().IsMainMenuVisible () )
        m_pSetCursorPosHook->CallSetCursorPos(X,Y);
}

bool CCore::GetRequiredDisplayResolution( int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex, bool& bOutAllowUnsafeResolutions )
{
    CVARS_GET( "show_unsafe_resolutions", bOutAllowUnsafeResolutions );
    return GetVideoModeManager()->GetRequiredDisplayResolution( iOutWidth, iOutHeight, iOutColorBits, iOutAdapterIndex );
}

bool CCore::GetDeviceSelectionEnabled( void )
{
    return GetApplicationSettingInt ( "device-selection-disabled" ) ? false : true;
}
