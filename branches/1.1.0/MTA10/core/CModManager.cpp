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
#include <shellapi.h>
#define DECLARE_PROFILER_SECTION_CModManager
#include "profiler/SharedUtil.Profiler.h"

using SharedUtil::CalcMTASAPath;

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                    );

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
    SetCrashHandlerFilter ( HandleExceptionGlobal );
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
    char szDllDirectory[ MAX_PATH + 1 ] = {'\0'};
    GetDllDirectory( sizeof ( szDllDirectory ), szDllDirectory );
    if ( stricmp( CalcMTASAPath ( "mta" ), szDllDirectory ) != 0 )
    {
        AddReportLog ( 3119, SString ( "DllDirectory wrong:  DllDirectory:'%s'  Path:'%s'", szDllDirectory, *CalcMTASAPath ( "mta" ) ) );
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
        CCore::GetSingleton ().ApplyFrameRateLimit ( 60 );      // Limit when not connected

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


long WINAPI CModManager::HandleExceptionGlobal ( _EXCEPTION_POINTERS* pException )
{
    // Create the exception information class
    CExceptionInformation_Impl* pExceptionInformation = new CExceptionInformation_Impl;
    pExceptionInformation->Set ( pException->ExceptionRecord->ExceptionCode, pException );

    // Grab the mod manager
    CModManager* pModManager = CModManager::GetSingletonPtr ();
    if ( pModManager )
    {
        // Got a client?
        if ( pModManager->m_pClientBase )
        {
            // Protect us from "double-faults"
            try
            {
                // Let the client handle it. If it could, continue the execution
                if ( pModManager->m_pClientBase->HandleException ( pExceptionInformation ) )
                {
                    // Delete the exception record and continue to search the exception stack
                    delete pExceptionInformation;
                    return EXCEPTION_CONTINUE_SEARCH;
                }

                // The client wants us to terminate the process
                DumpCoreLog ( pExceptionInformation );
                DumpMiniDump ( pException, pExceptionInformation );
                RunErrorTool ( pExceptionInformation );
                TerminateProcess ( GetCurrentProcess (), 1 );
            }
            catch ( ... )
            {
                // Double-fault, terminate the process
                DumpCoreLog ( pExceptionInformation );
                DumpMiniDump ( pException, pExceptionInformation );
                RunErrorTool ( pExceptionInformation );
                TerminateProcess ( GetCurrentProcess (), 1 );
            }
        }
        else
        {
            // Continue if we're in debug mode, if not terminate
            #ifdef MTA_DEBUG
                return EXCEPTION_CONTINUE_SEARCH;
            #endif
        }
    }

    // Terminate the process
    DumpCoreLog ( pExceptionInformation );
    DumpMiniDump ( pException, pExceptionInformation );
    RunErrorTool ( pExceptionInformation );
    TerminateProcess ( GetCurrentProcess (), 1 );
    return EXCEPTION_CONTINUE_SEARCH;
}


void CModManager::DumpCoreLog ( CExceptionInformation* pExceptionInformation )
{
    // Write a log with the generic exception information
    FILE* pFile = fopen ( CalcMTASAPath ( "mta\\core.log" ), "a+" );
    if ( pFile )
    {
        // Header
        fprintf ( pFile, "%s", "** -- Unhandled exception -- **\n\n" );

        // Write the time
        time_t timeTemp;
        time ( &timeTemp );

        SString strMTAVersionFull = SString ( "%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting ( "mta-version-ext" ).SplitRight ( ".", NULL, -2 ) );

        SString strInfo;
        strInfo += SString ( "Version = %s\n", strMTAVersionFull.c_str () );
        strInfo += SString ( "Time = %s", ctime ( &timeTemp ) );

        strInfo += SString ( "Module = %s\n", pExceptionInformation->GetModulePathName () );

        // Write the basic exception information
        strInfo += SString ( "Code = 0x%08X\n", pExceptionInformation->GetCode () );
        strInfo += SString ( "Offset = 0x%08X\n\n", pExceptionInformation->GetAddressModuleOffset () );

        // Write the register info
        strInfo += SString ( "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n" \
                         "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n" \
                         "CS=%04X   DS=%04X  SS=%04X  ES=%04X   " \
                         "FS=%04X  GS=%04X\n\n",
                         pExceptionInformation->GetEAX (),
                         pExceptionInformation->GetEBX (),
                         pExceptionInformation->GetECX (),
                         pExceptionInformation->GetEDX (),
                         pExceptionInformation->GetESI (),
                         pExceptionInformation->GetEDI (),
                         pExceptionInformation->GetEBP (),
                         pExceptionInformation->GetESP (),
                         pExceptionInformation->GetEIP (),
                         pExceptionInformation->GetEFlags (),
                         pExceptionInformation->GetCS (),
                         pExceptionInformation->GetDS (),
                         pExceptionInformation->GetSS (),
                         pExceptionInformation->GetES (),
                         pExceptionInformation->GetFS (),
                         pExceptionInformation->GetGS () );


        fprintf ( pFile, "%s", strInfo.c_str () );

        // End of unhandled exception
        fprintf ( pFile, "%s", "** -- End of unhandled exception -- **\n\n\n" );
        
        // Close the file
        fclose ( pFile );

        // For the crash dialog
        SetApplicationSetting ( "diagnostics", "last-crash-info", strInfo );
    }
}


void CModManager::DumpMiniDump ( _EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation )
{
    // Try to load the DLL in our directory
    HMODULE hDll = NULL;
    char szDbgHelpPath [MAX_PATH];
    if ( GetModuleFileName ( NULL, szDbgHelpPath, MAX_PATH ) )
    {
        char* pSlash = _tcsrchr ( szDbgHelpPath, '\\' );
        if ( pSlash )
        {
            _tcscpy ( pSlash + 1, "DBGHELP.DLL" );
            hDll = LoadLibrary ( szDbgHelpPath );
        }
    }

    // If we couldn't load the one in our dir, load any version available
    if ( !hDll )
    {
        hDll = LoadLibrary( "DBGHELP.DLL" );
    }

    // We could load a dll?
    if ( hDll )
    {
        // Grab the MiniDumpWriteDump proc address
        MINIDUMPWRITEDUMP pDump = reinterpret_cast < MINIDUMPWRITEDUMP > ( GetProcAddress( hDll, "MiniDumpWriteDump" ) );
        if ( pDump )
        {
            // Create the file
            HANDLE hFile = CreateFile ( CalcMTASAPath ( "mta\\core.dmp" ), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            if ( hFile != INVALID_HANDLE_VALUE )
            {
                // Create an exception information struct
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
                ExInfo.ThreadId = GetCurrentThreadId ();
                ExInfo.ExceptionPointers = pException;
                ExInfo.ClientPointers = FALSE;

                // Write the dump
                pDump ( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );

                // Close the dumpfile
                CloseHandle ( hFile );

                // Grab the current time
                // Ask windows for the system time.
                SYSTEMTIME SystemTime;
                GetLocalTime ( &SystemTime );

                // Create the dump directory
                CreateDirectory ( CalcMTASAPath ( "mta\\dumps" ), 0 );

                SString strModuleName = pExceptionInformation->GetModuleBaseName ();
                strModuleName = strModuleName.ReplaceI ( ".dll", "" ).Replace ( ".exe", "" ).Replace ( "_", "" ).Replace ( ".", "" ).Replace ( "-", "" );
                if ( strModuleName.length () == 0 )
                    strModuleName = "unknown";

                SString strMTAVersionFull = SString ( "%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting ( "mta-version-ext" ).SplitRight ( ".", NULL, -2 ) );
                SString strSerialPart = GetApplicationSetting ( "serial" ).substr ( 0, 5 );
                uint uiServerIP = GetApplicationSettingInt ( "last-server-ip" );
                uint uiServerPort = GetApplicationSettingInt ( "last-server-port" );
                int uiServerTime = GetApplicationSettingInt ( "last-server-time" );
                int uiServerDuration = _time32 ( NULL ) - uiServerTime;
                uiServerDuration = Clamp ( 0, uiServerDuration + 1, 0xfff );

                // Get path to mta dir
                SString strPathCode;
                {
                    std::vector < SString > parts;
                    PathConform ( CalcMTASAPath ( "" ) ).Split ( PATH_SEPERATOR, parts );
                    for ( uint i = 0 ; i < parts.size () ; i++ )
                    {
                        if ( parts[i].CompareI ( "Program Files" ) )
                            strPathCode += "Pr";
                        else
                        if ( parts[i].CompareI ( "Program Files (x86)" ) )
                            strPathCode += "Px";
                        else
                        if ( parts[i].CompareI ( "MTA San Andreas" ) )
                            strPathCode += "Mt";
                        else
                        if ( parts[i].BeginsWithI ( "MTA San Andreas" ) )
                            strPathCode += "Mb";
                        else
                            strPathCode += parts[i].Left ( 1 ).ToUpper ();
                    }
                }

                // Ensure filename parts match up with EDumpFileNameParts
                SString strFilename ( "mta\\dumps\\client_%s_%s_%08x_%x_%s_%08X_%04X_%03X_%s_%04d%02d%02d_%02d%02d.dmp",
                                             strMTAVersionFull.c_str (),
                                             strModuleName.c_str (),
                                             pExceptionInformation->GetAddressModuleOffset (),
                                             pExceptionInformation->GetCode () & 0xffff,
                                             strPathCode.c_str (),
                                             uiServerIP,
                                             uiServerPort,
                                             uiServerDuration,
                                             strSerialPart.c_str (),
                                             SystemTime.wYear,
                                             SystemTime.wMonth,
                                             SystemTime.wDay,
                                             SystemTime.wHour,
                                             SystemTime.wMinute
                                           );

                SString strPathFilename = CalcMTASAPath ( strFilename );

                // Copy the file
                CopyFile ( CalcMTASAPath ( "mta\\core.dmp" ), strPathFilename, false );

                // For the dump uploader
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "none" );
                SetApplicationSetting ( "diagnostics", "last-dump-save", strPathFilename );

                // Try to append pool sizes info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-pools" );
                CBuffer poolInfo;
                GetPoolInfo ( poolInfo );
                AppendToDumpFile ( strPathFilename, poolInfo, 'POLs', 'POLe' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-pools" );

                // Try to append d3d state info to dump file
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "try-d3d" );
                CBuffer d3dInfo;
                GetD3DInfo ( d3dInfo );
                AppendToDumpFile ( strPathFilename, d3dInfo, 'D3Ds', 'D3De' );
                SetApplicationSetting ( "diagnostics", "last-dump-extra", "added-d3d" );
            }
        }

        // Free the DLL again
        FreeLibrary ( hDll );
    }
}


void CModManager::RunErrorTool ( CExceptionInformation* pExceptionInformation )
{
// MTA Error Reporter is now integrated into the launcher

    // Only do once
    static bool bDoneReport = false;
    if ( bDoneReport )
        return;
    bDoneReport = false;

    // Log the basic exception information
    SString strMessage ( "Crash 0x%08X 0x%08X %s"
                         " EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESI=%08X"
                         " EDI=%08X EBP=%08X ESP=%08X EIP=%08X FLG=%08X"
                         " CS=%04X DS=%04X SS=%04X ES=%04X"
                         " FS=%04X GS=%04X",
                         pExceptionInformation->GetCode (),
                         pExceptionInformation->GetAddressModuleOffset (),
                         pExceptionInformation->GetModulePathName (),
                         pExceptionInformation->GetEAX (),
                         pExceptionInformation->GetEBX (),
                         pExceptionInformation->GetECX (),
                         pExceptionInformation->GetEDX (),
                         pExceptionInformation->GetESI (),
                         pExceptionInformation->GetEDI (),
                         pExceptionInformation->GetEBP (),
                         pExceptionInformation->GetESP (),
                         pExceptionInformation->GetEIP (),
                         pExceptionInformation->GetEFlags (),
                         pExceptionInformation->GetCS (),
                         pExceptionInformation->GetDS (),
                         pExceptionInformation->GetSS (),
                         pExceptionInformation->GetES (),
                         pExceptionInformation->GetFS (),
                         pExceptionInformation->GetGS ()
                        );

    AddReportLog ( 3120, strMessage );

    // Try relaunch with crashed flag
    SString strMTASAPath = GetMTASABaseDir ();
    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

#ifdef MTA_DEBUG
    #define MTA_EXE_NAME            "Multi Theft Auto_d.exe"
#else
    #define MTA_EXE_NAME            "Multi Theft Auto.exe"
#endif
    SString strFile = strMTASAPath + "\\" + MTA_EXE_NAME;
    ShellExecute( NULL, "open", strFile, "install_stage=crashed", NULL, SW_SHOWNORMAL );
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
                CLogger::GetSingleton ().ErrorPrintf ( "Unknown mod DLL: %s", szName );
            }

            // Free the DLL
            FreeLibrary ( hDLL );
        }
        else
        {
            CLogger::GetSingleton ().ErrorPrintf ( "Invalid mod DLL: %s (reason: %d)", szName, GetLastError() );
        }
    }
}


//
// Add extra data to the dump file and hope visual studio doesn't mind
//
void CModManager::AppendToDumpFile ( const SString& strPathFilename, const CBuffer& dataBuffer, DWORD dwMagicStart, DWORD dwMagicEnd )
{
    CBuffer output;
    CBufferWriteStream stream ( output );
    // 4 bytes zero
    stream.Write ( (DWORD)0 );
    // 4 bytes magic
    stream.Write ( dwMagicStart );
    // 4 bytes size of data
    stream.Write ( dataBuffer.GetSize () );
    // n bytes data
    stream.WriteBytes ( dataBuffer.GetData (), dataBuffer.GetSize () );
    // 4 bytes size of data
    stream.Write ( dataBuffer.GetSize () );
    // 4 bytes magic
    stream.Write ( dwMagicEnd );
    // 4 bytes zero
    stream.Write ( (DWORD)0 );
    FileAppend ( strPathFilename, output.GetData (), output.GetSize () );
}


//
// Helper crap for GetPoolInfo
//
namespace
{
    #define CLASS_CBuildingPool                 0xb74498
    #define CLASS_CPedPool                      0xb74490
    #define CLASS_CObjectPool                   0xb7449c
    #define CLASS_CDummyPool                    0xb744a0
    #define CLASS_CVehiclePool                  0xb74494
    #define CLASS_CColModelPool                 0xb744a4
    #define CLASS_CTaskPool                     0xb744a8
    #define CLASS_CEventPool                    0xb744ac
    #define CLASS_CTaskAllocatorPool            0xb744bc
    #define CLASS_CPedIntelligencePool          0xb744c0
    #define CLASS_CPedAttractorPool             0xb744c4
    #define CLASS_CEntryInfoNodePool            0xb7448c
    #define CLASS_CNodeRoutePool                0xb744b8
    #define CLASS_CPatrolRoutePool              0xb744b4
    #define CLASS_CPointRoutePool               0xb744b0
    #define CLASS_CPtrNodeDoubleLinkPool        0xB74488
    #define CLASS_CPtrNodeSingleLinkPool        0xB74484

    #define FUNC_CBuildingPool_GetNoOfUsedSpaces                0x550620
    #define FUNC_CPedPool_GetNoOfUsedSpaces                     0x5504A0
    #define FUNC_CObjectPool_GetNoOfUsedSpaces                  0x54F6B0
    #define FUNC_CDummyPool_GetNoOfUsedSpaces                   0x5507A0
    #define FUNC_CVehiclePool_GetNoOfUsedSpaces                 0x42CCF0
    #define FUNC_CColModelPool_GetNoOfUsedSpaces                0x550870
    #define FUNC_CTaskPool_GetNoOfUsedSpaces                    0x550940
    #define FUNC_CEventPool_GetNoOfUsedSpaces                   0x550A10
    #define FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces           0x550d50
    #define FUNC_CPedIntelligencePool_GetNoOfUsedSpaces         0x550E20
    #define FUNC_CPedAttractorPool_GetNoOfUsedSpaces            0x550ef0
    #define FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces           0x5503d0
    #define FUNC_CNodeRoutePool_GetNoOfUsedSpaces               0x550c80
    #define FUNC_CPatrolRoutePool_GetNoOfUsedSpaces             0x550bb0
    #define FUNC_CPointRoutePool_GetNoOfUsedSpaces              0x550ae0
    #define FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces       0x550230
    #define FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces       0x550300

    int GetPoolCapacity ( ePools pool )
    {
        DWORD iPtr = NULL;
        DWORD cPtr = NULL;
        switch ( pool )
        {
            case BUILDING_POOL:             iPtr = 0x55105F; break;
            case PED_POOL:                  iPtr = 0x550FF2; break;
            case OBJECT_POOL:               iPtr = 0x551097; break;
            case DUMMY_POOL:                iPtr = 0x5510CF; break;
            case VEHICLE_POOL:              cPtr = 0x55102A; break;
            case COL_MODEL_POOL:            iPtr = 0x551107; break;
            case TASK_POOL:                 iPtr = 0x55113F; break;
            case EVENT_POOL:                iPtr = 0x551177; break;
            case TASK_ALLOCATOR_POOL:       cPtr = 0x55124E; break;
            case PED_INTELLIGENCE_POOL:     iPtr = 0x551283; break;
            case PED_ATTRACTOR_POOL:        cPtr = 0x5512BC; break;
            case ENTRY_INFO_NODE_POOL:      iPtr = 0x550FBA; break;
            case NODE_ROUTE_POOL:           cPtr = 0x551219; break;
            case PATROL_ROUTE_POOL:         cPtr = 0x5511E4; break;
            case POINT_ROUTE_POOL:          cPtr = 0x5511AF; break;
            case POINTER_DOUBLE_LINK_POOL:  iPtr = 0x550F82; break;
            case POINTER_SINGLE_LINK_POOL:  iPtr = 0x550F46; break;
        }
        if ( iPtr )
            return *(int*)iPtr;

        if ( cPtr )
            return *(char*)cPtr;

        return 0;
    }

    int GetNumberOfUsedSpaces ( ePools pool )
    {
        DWORD dwFunc = NULL;
        DWORD dwThis = NULL;
        switch ( pool )
        {
            case BUILDING_POOL: dwFunc = FUNC_CBuildingPool_GetNoOfUsedSpaces; dwThis = CLASS_CBuildingPool; break;
            case PED_POOL: dwFunc = FUNC_CPedPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedPool; break;
            case OBJECT_POOL: dwFunc = FUNC_CObjectPool_GetNoOfUsedSpaces; dwThis = CLASS_CObjectPool; break;
            case DUMMY_POOL: dwFunc = FUNC_CDummyPool_GetNoOfUsedSpaces; dwThis = CLASS_CDummyPool; break;
            case VEHICLE_POOL: dwFunc = FUNC_CVehiclePool_GetNoOfUsedSpaces; dwThis = CLASS_CVehiclePool; break;
            case COL_MODEL_POOL: dwFunc = FUNC_CColModelPool_GetNoOfUsedSpaces; dwThis = CLASS_CColModelPool; break;
            case TASK_POOL: dwFunc = FUNC_CTaskPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskPool; break;
            case EVENT_POOL: dwFunc = FUNC_CEventPool_GetNoOfUsedSpaces; dwThis = CLASS_CEventPool; break;
            case TASK_ALLOCATOR_POOL: dwFunc = FUNC_CTaskAllocatorPool_GetNoOfUsedSpaces; dwThis = CLASS_CTaskAllocatorPool; break;
            case PED_INTELLIGENCE_POOL: dwFunc = FUNC_CPedIntelligencePool_GetNoOfUsedSpaces; dwThis = CLASS_CPedIntelligencePool; break;
            case PED_ATTRACTOR_POOL: dwFunc = FUNC_CPedAttractorPool_GetNoOfUsedSpaces; dwThis = CLASS_CPedAttractorPool; break;
            case ENTRY_INFO_NODE_POOL: dwFunc = FUNC_CEntryInfoNodePool_GetNoOfUsedSpaces; dwThis = CLASS_CEntryInfoNodePool; break;
            case NODE_ROUTE_POOL: dwFunc = FUNC_CNodeRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CNodeRoutePool; break;
            case PATROL_ROUTE_POOL: dwFunc = FUNC_CPatrolRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPatrolRoutePool; break;
            case POINT_ROUTE_POOL: dwFunc = FUNC_CPointRoutePool_GetNoOfUsedSpaces; dwThis = CLASS_CPointRoutePool; break;
            case POINTER_DOUBLE_LINK_POOL: dwFunc = FUNC_CPtrNodeDoubleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeDoubleLinkPool; break;
            case POINTER_SINGLE_LINK_POOL: dwFunc = FUNC_CPtrNodeSingleLinkPool_GetNoOfUsedSpaces; dwThis = CLASS_CPtrNodeSingleLinkPool; break;
            default: return -1;
        }

        int iOut = -2;
        if ( *(DWORD *)dwThis != NULL )
        {
            _asm
            {
                mov     ecx, dwThis
                mov     ecx, [ecx]
                call    dwFunc
                mov     iOut, eax

            }
        }

        return iOut;
    }
}


//
// Grab the state of the memory pools
//
void CModManager::GetPoolInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write PoolInfo version
    stream.Write ( 1 );

    // Write number of pools we have info on
    stream.Write ( MAX_POOLS );

    // For each pool
    for ( int i = 0; i < MAX_POOLS ; i++ )
    {
        int iCapacity = GetPoolCapacity ( (ePools)i );
        int iUsedSpaces = GetNumberOfUsedSpaces ( (ePools)i );
        // Write pool info
        stream.Write ( i );
        stream.Write ( iCapacity );
        stream.Write ( iUsedSpaces );
    }

    // TODO - Get usage info for other arrays such as CMatrixLinkList
}


//
// Grab the state of D3D
//
void CModManager::GetD3DInfo ( CBuffer& buffer )
{
    CBufferWriteStream stream ( buffer );

    // Write D3DInfo version
    stream.Write ( 1 );

    // Quit if device state pointer is not valid
    if ( !g_pDeviceState )
        return;

    // Write D3D call type
    stream.Write ( g_pDeviceState->CallState.callType );

    // Only record state if crash was inside D3D
    if ( g_pDeviceState->CallState.callType == CProxyDirect3DDevice9::SCallState::NONE )
        return;

    // Write D3D call args
    stream.Write ( g_pDeviceState->CallState.uiNumArgs );
    for ( uint i = 0; i < g_pDeviceState->CallState.uiNumArgs ; i++ )
        stream.Write ( g_pDeviceState->CallState.args[i] );

    // Try to get CRenderWare pointer
    CCore* pCore = CCore::GetSingletonPtr ();
    CGame* pGame = pCore ? pCore->GetGame () : NULL;
    CRenderWare* pRenderWare = pGame->GetRenderWare ();
    // Write on how we got on with doing that
    stream.Write ( (uchar)( pCore ? 1 : 0 ) );
    stream.Write ( (uchar)( pGame ? 1 : 0 ) );
    stream.Write ( (uchar)( pRenderWare ? 1 : 0 ) );

    // Write last used texture D3D pointer
    stream.Write ( (uint)g_pDeviceState->TextureState[0].Texture );

    // Write last used texture name
    SString strTextureName = "no name";
    if ( pRenderWare )
        strTextureName = pRenderWare->GetTextureName ( (CD3DDUMMY*)g_pDeviceState->TextureState[0].Texture );
    stream.WriteString ( strTextureName );
}
