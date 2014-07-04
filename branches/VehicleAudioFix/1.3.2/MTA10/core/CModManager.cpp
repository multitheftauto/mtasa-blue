/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManager.cpp
*  PURPOSE:     Game mod loading manager
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#define DECLARE_PROFILER_SECTION_CModManager
#include "profiler/SharedUtil.Profiler.h"

using SharedUtil::CalcMTASAPath;

template<> CModManager * CSingleton < CModManager > ::m_pSingleton = NULL;

CModManager::CModManager ( void )
{
    // Init
    m_hClientDLL = NULL;
    m_pClientBase = NULL;
    m_bUnloadRequested = false;

    // Default mod name defaults to "default"
    m_strDefaultModName = "default";

    // Load the modlist from the folders in "mta/mods"
    InitializeModList ( CalcMTASAPath( "mods\\" ) );

    // Set up our exception handler
    #ifndef MTA_DEBUG
    SetCrashHandlerFilter ( CCrashDumpWriter::HandleExceptionGlobal );
    #endif
}

CModManager::~CModManager ( void )
{
    // Unload the current loaded mod (if loaded)
    Unload ();

    // Clear the modlist
    Clear ();

}


void CModManager::RequestLoad ( const char* szModName, const char* szArguments )
{
    // Reset an eventual old request (and free our strings)
    ClearRequest ();

    // An unload is now requested
    m_bUnloadRequested = true;

    // Requested a mod name?
    if ( szModName )
    {
        // Store it
        m_strRequestedMod = szModName;

        // Arguments?
        m_strRequestedModArguments = szArguments ? szArguments : "";
    }
}


void CModManager::RequestLoadDefault ( const char* szArguments )
{
    RequestLoad ( m_strDefaultModName.c_str (), szArguments );
}


void CModManager::RequestUnload ( void )
{
    RequestLoad ( NULL, NULL );
    CCore::GetSingletonPtr () -> OnModUnload ();
}


void CModManager::ClearRequest ( void )
{
    // Free the old mod name
    m_strRequestedMod = "";

    // Free the old mod arguments
    m_strRequestedModArguments = "";

    // No unload requested now
    m_bUnloadRequested = false;
}


bool CModManager::IsLoaded ( void )
{
    return ( m_hClientDLL != NULL );
}


CClientBase* CModManager::Load ( const char* szName, const char* szArguments )
{
    // Make sure we haven't already loaded a mod
    Unload ();

    // Get the entry for the given name
    std::map < std::string, std::string >::iterator itMod = m_ModDLLFiles.find ( szName );
    if ( itMod == m_ModDLLFiles.end () )
    {
        CCore::GetSingleton ().GetConsole ()->Printf ( "Unable to load %s (unknown mod)", szName );
        return NULL;
    }

    // Ensure DllDirectory has not been changed
    SString strDllDirectory = GetSystemDllDirectory();
    if ( CalcMTASAPath ( "mta" ).CompareI ( strDllDirectory ) == false )
    {
        AddReportLog ( 3119, SString ( "DllDirectory wrong:  DllDirectory:'%s'  Path:'%s'", *strDllDirectory, *CalcMTASAPath ( "mta" ) ) );
        SetDllDirectory( CalcMTASAPath ( "mta" ) );
    }
    

    // Load the library and use the supplied path as an extra place to search for dependencies
    m_hClientDLL = LoadLibraryEx ( itMod->second.c_str (), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
    if ( !m_hClientDLL )
    {
        DWORD dwError = GetLastError ();
        char szError [ 2048 ];
        char* p;

        FormatMessage ( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                        NULL, dwError, LANG_NEUTRAL, szError, sizeof ( szError ), NULL );

        // Remove newlines from the error message
        p = szError + strlen ( szError ) - 1;
        while ( p >= szError && (*p == '\r' || *p == '\n' ) )
        {
            *p = '\0';
            --p;
        }

        CCore::GetSingleton ().GetConsole ()->Printf ( "Unable to load %s's DLL (reason: %s)", szName, szError );
        return NULL;
    }

    // Get the address of InitClient
    typedef CClientBase* (__cdecl pfnClientInitializer) ( void );     /* FIXME: Should probably not be here */

    pfnClientInitializer* pClientInitializer = reinterpret_cast < pfnClientInitializer* > ( GetProcAddress ( m_hClientDLL, "InitClient" ) );
    if ( pClientInitializer == NULL )
    {
        CCore::GetSingleton ().GetConsole ()->Printf ( "Unable to load %s's DLL (unknown mod)", szName, GetLastError () );
        FreeLibrary ( m_hClientDLL );
        return NULL;
    }

    // Call InitClient and store the Client interface in m_pClientBase
    m_pClientBase = pClientInitializer ();

    // Call the client base initializer
    if ( !m_pClientBase ||
         m_pClientBase->ClientInitialize ( szArguments, CCore::GetSingletonPtr () ) != 0 )
    {
        CCore::GetSingleton ().GetConsole ()->Printf ( "Unable to load %s's DLL (unable to init, bad version?)", szName, GetLastError () );
        FreeLibrary ( m_hClientDLL );
        return NULL;
    }

    // HACK: make the console input active if its visible
    if ( CLocalGUI::GetSingleton ().IsConsoleVisible () )
        CLocalGUI::GetSingleton ().GetConsole ()->ActivateInput ();

    // Tell chat to start handling input
    CLocalGUI::GetSingleton ().GetChat ()->OnModLoad ();
 
    // Return the interface
    return m_pClientBase;
}


void CModManager::Unload ( void )
{
    // If a mod is loaded, we call m_pClientBase->ClientShutdown and then free the library
    if ( m_hClientDLL != NULL )
    {
        // Call m_pClientBase->ClientShutdown
        if ( m_pClientBase )
        {
            m_pClientBase->ClientShutdown ();
            m_pClientBase = NULL;
        }

        // Unregister the commands it had registered
        CCore::GetSingleton ().GetCommands ()->DeleteAll ();

        // Stop all screen grabs
        CGraphics::GetSingleton ().GetScreenGrabber ()->ClearScreenShotQueue ();

        // Free the Client DLL
        FreeLibrary ( m_hClientDLL );
        m_hClientDLL = NULL;

        // Call the on mod unload func
        CCore::GetSingletonPtr () -> OnModUnload ();

        // Reset chatbox status (so it won't prevent further input), and clear it
        /*CLocalGUI::GetSingleton ().GetChatBox ()->SetInputEnabled ( false );
        CLocalGUI::GetSingleton ().GetChatBox ()->Clear ();*/
        CLocalGUI::GetSingleton ().GetChat ()->SetInputVisible ( false );
        CLocalGUI::GetSingleton ().GetChat ()->Clear ();
        CLocalGUI::GetSingleton ().SetChatBoxVisible ( true );

        // Reset the debugview status
        CLocalGUI::GetSingleton ().GetDebugView ()->SetVisible ( false );
        CLocalGUI::GetSingleton ().GetDebugView ()->Clear ();
        CLocalGUI::GetSingleton ().SetDebugViewVisible ( false );

        // NULL the message processor and the unhandled command handler
        CCore::GetSingleton ().SetClientMessageProcessor ( NULL );
        CCore::GetSingleton ().GetCommands ()->SetExecuteHandler ( NULL );

        // Reset cursor alpha
        CCore::GetSingleton ().GetGUI ()->SetCursorAlpha ( 1.0f, true );

        // Reset the modules
        CCore::GetSingleton ().GetGame ()->Reset ();
        CCore::GetSingleton ().GetMultiplayer ()->Reset ();
        CCore::GetSingleton ().GetNetwork ()->Reset ();
        assert ( CCore::GetSingleton ().GetNetwork ()->GetServerBitStreamVersion () == 0 );

        // Enable the console again
        CCore::GetSingleton ().GetConsole ()->SetEnabled ( true );

        // Force the mainmenu back
        CCore::GetSingleton ().SetConnected ( false );
        CLocalGUI::GetSingleton ().GetMainMenu ()->SetIsIngame ( false );
        CLocalGUI::GetSingleton ().GetMainMenu ()->SetVisible ( true, false );

        if ( XfireIsLoaded () )
        {
            XfireSetCustomGameData ( 0, NULL, NULL ); 
        }
    }
}


void CModManager::DoPulsePreFrame ( void )
{
    if ( m_pClientBase )
    {
        m_pClientBase->PreFrameExecutionHandler ();
    }
}


void CModManager::DoPulsePreHUDRender ( bool bDidUnminimize, bool bDidRecreateRenderTargets )
{
    if ( m_pClientBase )
    {
        m_pClientBase->PreHUDRenderExecutionHandler ( bDidUnminimize, bDidRecreateRenderTargets );
    }
}


void CModManager::DoPulsePostFrame ( void )
{
    // Load/unload requested?
    if ( m_bUnloadRequested )
    {
        // Unload the current mod
        Unload ();

        // Load a new mod?
        if ( m_strRequestedMod != "" )
        {
            Load ( m_strRequestedMod, m_strRequestedModArguments );
        }

        // Clear the request
        ClearRequest ();
    }

    // Pulse the client
    if ( m_pClientBase )
    {
        m_pClientBase->PostFrameExecutionHandler ();
    }
    else
    {
        CCore::GetSingleton ().GetNetwork ()->DoPulse ();
    }

    // Make sure frame rate limit gets applied
    if ( m_pClientBase )
        CCore::GetSingleton ().EnsureFrameRateLimitApplied ();  // Catch missed frames
    else
        CCore::GetSingleton ().ApplyFrameRateLimit ( 88 );      // Limit when not connected

    // Load/unload requested?
    if ( m_bUnloadRequested )
    {
        // Unload the current mod
        Unload ();

        // Load a new mod?
        if ( m_strRequestedMod != "" )
        {
            Load ( m_strRequestedMod, m_strRequestedModArguments );
        }

        // Clear the request
        ClearRequest ();
    }
}


CClientBase* CModManager::GetCurrentMod ( void )
{
    return m_pClientBase;
}

void CModManager::RefreshMods ( void )
{
    // Clear the list, and load it again
    Clear ();
    InitializeModList ( CalcMTASAPath( "mods\\" ) );
}


void CModManager::InitializeModList ( const char* szModFolderPath )
{
    // Variables used to search the mod directory
    WIN32_FIND_DATAA FindData;
    HANDLE hFind;

    // Allocate a string with length of path + 5 letters to store searchpath plus "\*.*"
    SString strPathWildchars ( "%s*.*", szModFolderPath );

    // Set the working directory to the MTA folder
    CFilePathTranslator filePathTranslator;
    filePathTranslator.SetCurrentWorkingDirectory ( "mta" );

    // Create a search
    hFind = FindFirstFile ( strPathWildchars, &FindData );

    // If we found a first file ...
    if ( hFind != INVALID_HANDLE_VALUE )
    {
        // Add it to the list
        VerifyAndAddEntry ( szModFolderPath, FindData.cFileName );

        // Search until there aren't any files left
        while ( FindNextFile ( hFind, &FindData ) == TRUE )
        {
            VerifyAndAddEntry ( szModFolderPath, FindData.cFileName );
        }

        // End the search
        FindClose ( hFind );
    }

    // Reset the working directory
    filePathTranslator.UnSetCurrentWorkingDirectory ();
}


void CModManager::Clear ( void )
{
    // Clear the list
    m_ModDLLFiles.clear ();
}


void CModManager::VerifyAndAddEntry ( const char* szModFolderPath, const char* szName )
{
    // Name musn't be a . or .. link or we might load unwanted libraries
    // Hack: Also skip race for now as it will crash the game!
    if ( ( strcmp ( szName, "." ) != 0) && 
         ( strcmp ( szName, ".." ) != 0 ) &&
         ( stricmp ( szName, "race" ) != 0 ) )
    {
        // Put together a modpath string and a MTA-relative path to Client(_d).dll
        SString strClientDLL ( "%s%s\\%s", szModFolderPath, szName, CMODMANAGER_CLIENTDLL );

        // Attempt to load the primary client DLL
        HMODULE hDLL = LoadLibraryEx ( strClientDLL, NULL, DONT_RESOLVE_DLL_REFERENCES );
        if (hDLL != 0)
        {
            // Check if InitClient symbol exists
            if ( GetProcAddress ( hDLL, "InitClient" ) != NULL )
            {
                // Add it to the list
                m_ModDLLFiles [ szName ] = strClientDLL;
            }
            else
            {
                WriteErrorEvent( SString( "Unknown mod DLL: %s", szName ) );
            }

            // Free the DLL
            FreeLibrary ( hDLL );
        }
        else
        {
            WriteErrorEvent( SString( "Invalid mod DLL: %s (reason: %d)", szName, GetLastError() ) );
        }
    }
}

