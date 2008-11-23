/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CServer.cpp
*  PURPOSE:     Local server instancing class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

static volatile bool g_bIsStarted = false;
extern CCoreInterface* g_pCore;
CCriticalSection CServer::m_OutputCC;
std::list < std::string > CServer::m_OutputQueue;

#ifdef MTA_DEBUG
    #define SERVER_DLL_PATH "core_d.dll"
#else
    #define SERVER_DLL_PATH "core.dll"
#endif

#define ERROR_NO_ERROR 0
#define ERROR_NO_NETWORK_LIBRARY 1
#define ERROR_NETWORK_LIBRARY_FAILED 2
#define ERROR_LOADING_MOD 3

char* szServerErrors[4] =
{
    "Server stopped",
    "Could not load network library",
    "Loading network library failed",
    "Error loading mod"
};

CServer::CServer ( void )
{
    assert ( !g_bIsStarted );
    m_szConfig = NULL;

    // Initialize
    m_bIsReady = false;
    m_pLibrary = NULL;
    m_hThread = INVALID_HANDLE_VALUE;
    m_iLastError = ERROR_NO_ERROR;

    // Grab the path to the MTA installation folder
    strncpy ( m_szServerRoot, g_pCore->GetInstallRoot (), MAX_PATH );
    size_t sizeInstallRoot = strlen ( m_szServerRoot );

    // Append / if neccessary
    if ( m_szServerRoot [sizeInstallRoot - 1] != '/' ||
         m_szServerRoot [sizeInstallRoot - 1] != '\\' )
    {
        m_szServerRoot [sizeInstallRoot] = '/';
    }

    // Append server/ to it
    strncat ( m_szServerRoot, "server/", MAX_PATH );

    // Append the server core dll name to it
    snprintf ( m_szDLLFile, MAX_PATH, "%s%s", m_szServerRoot, SERVER_DLL_PATH );
}


CServer::~CServer ( void )
{
    // Make sure the server is stopped
    Stop ();

    // Make sure the thread handle is closed
    if ( m_hThread != INVALID_HANDLE_VALUE )
    {
        CloseHandle ( m_hThread );
    }
}


void CServer::DoPulse ( void )
{
    if ( IsRunning () )
    {
        // Make sure the server doesn't happen to be adding anything right now
        m_OutputCC.Lock ();

        // Anything to output to console?
        if ( m_OutputQueue.size () > 0 )
        {
            // Loop through our output queue and echo it to console
            std::list < std::string >::const_iterator iter = m_OutputQueue.begin ();
            for ( ; iter != m_OutputQueue.end (); iter++ )
            {
                // Echo it
                const char* szString = iter->c_str ();
                g_pCore->GetConsole ()->Echo ( szString );

                // Does the message end with "Server started and is ready to accept connections!\n"?
                size_t sizeString = iter->length ();
                if ( sizeString >= 51 &&
                    stricmp ( szString + sizeString - 51, "Server started and is ready to accept connections!\n" ) == 0 )
                {
                    m_bIsReady = true;
                }
            }

            // Clear the list
            m_OutputQueue.clear ();
        }
        // Unlock again
        m_OutputCC.Unlock ();
    }
    else
    {
        // not running, errored?
        if ( GetLastError() != ERROR_NO_ERROR )
        {
            Stop ();
        }
    }
}


bool CServer::Start ( const char* szConfig )
{
    // Not already started?
    if ( !g_bIsStarted )
    {
        m_szConfig = new char [ strlen ( szConfig ) + 1 ];
	    strcpy ( m_szConfig, szConfig );

        // Check that the DLL exists
        if ( !DoesFileExist ( m_szDLLFile ) )
        {
            g_pCore->GetConsole ()->Printf ( "Unable to find: '%s'", m_szDLLFile );
            return false;
        }

        // We're now started, but not ready
        g_bIsStarted = true;
        m_bIsReady = false;

        // Close the previous thread?
        if ( m_hThread != INVALID_HANDLE_VALUE )
        {
            CloseHandle ( m_hThread );
        }

        // Create a thread to run the server
        DWORD dwTemp;
        m_hThread = CreateThread ( NULL, 0, Thread_EntryPoint, this, 0, &dwTemp );
        return m_hThread != NULL;
    }

    return false;
}


bool CServer::IsStarted ( void )
{
    return g_bIsStarted;
}


bool CServer::Stop ( void )
{
    // Started?
    if ( g_bIsStarted )
    {
        // Wait for the library to come true or is started to go false
        // This is so a call to Start then fast call to Stop will work. Otherwize it might not
        // get time to start the server thread before we terminate it and we will end up
        // starting it after this call to Stop.
        while ( g_bIsStarted && !m_pLibrary )
        {
            Sleep ( 1 );
        }

        // Lock
        m_CriticalSection.Lock ();

        // Is the server running?
        if ( m_pLibrary )
        {
            // Send the exit message
            Send ( "exit" );
        }

        // Unlock it so we won't deadlock
        m_CriticalSection.Unlock ();
    }

    // If we have a thread, wait for it to finish
    if ( m_hThread != INVALID_HANDLE_VALUE )
    {
        // Let the thread finish
        WaitForSingleObject ( m_hThread, INFINITE );

        // If we can get an exit code, see if it's non-zero
        DWORD dwExitCode = 0;
        if ( GetExitCodeThread ( m_hThread, &dwExitCode ) )
        {
            // Non-zero, output error
            if ( dwExitCode != 0 )
            {
                g_pCore->ShowMessageBox ( "Error", "Could not start the local server. See console for details.", MB_BUTTON_OK | MB_ICON_ERROR );
                g_pCore->GetConsole ()->Printf ( "Error: Could not start local server. [%s]", szServerErrors[GetLastError()] );
            }
        }

        // Close it
        CloseHandle ( m_hThread );
        m_hThread = INVALID_HANDLE_VALUE;
    }

    m_iLastError = ERROR_NO_ERROR;

    return true;
}


bool CServer::Send ( const char* szString )
{
    // Server running?
    bool bReturn = false;
    if ( g_bIsStarted )
    {
        // Wait for the library to come true or is started to go false
        while ( g_bIsStarted && !m_pLibrary )
        {
            Sleep ( 1 );
        } 

        // Lock
        m_CriticalSection.Lock ();

        // Are we running the server
        if ( m_pLibrary )
        {
            // Grab the SendServerCommand function pointer
            typedef bool ( SendServerCommand_t )( const char* );
            SendServerCommand_t* pfnSendServerCommand = reinterpret_cast < SendServerCommand_t* > ( m_pLibrary->GetProcedureAddress ( "SendServerCommand" ) );
            if ( pfnSendServerCommand )
            {
                // Call it with the command
                bReturn = pfnSendServerCommand ( szString );
            }
        }

        // Unlock
        m_CriticalSection.Unlock ();
    }

    // Return
    return bReturn;
}


DWORD WINAPI CServer::Thread_EntryPoint ( LPVOID pThis )
{
    return reinterpret_cast < CServer* > ( pThis )->Thread_Run ();
}


unsigned long CServer::Thread_Run ( void )
{
    // Enter critical section
    m_CriticalSection.Lock ();

    // Already loaded? Just return or we get a memory leak.
    if ( m_pLibrary )
    {
        m_CriticalSection.Unlock ();
        return 0;
    }

    // Load the DLL
    m_pLibrary = new CDynamicLibrary;
    if ( m_pLibrary->Load ( m_szDLLFile ) )
    {
        // Grab the entrypoint
        typedef int ( Main_t )( int, char* [] );
        Main_t* pfnEntryPoint = reinterpret_cast < Main_t* > ( m_pLibrary->GetProcedureAddress ( "Run" ) );
        if ( pfnEntryPoint )
        {
            // Populate the arguments array
            char szArgument1 [8];
            strcpy ( szArgument1, "-D" );

            char szArgument2 [256];
            strncpy ( szArgument2, m_szServerRoot, 256 );
            szArgument2 [ 255 ] = 0;

            char szArgument3 [16];
            strcpy ( szArgument3, "--config" );

            char szArgument4 [64];
            strcpy ( szArgument4, m_szConfig );

            char szArgument5 [8];
            strcpy ( szArgument5, "-s" );

            char* szArguments [7];
            szArguments [0] = szArgument1;
            szArguments [1] = szArgument2;
            szArguments [2] = szArgument3;
            szArguments [3] = szArgument4;
            szArguments [4] = szArgument5;

            // Give the server our function to output stuff to the console
            char szArgument6 [32];
            strcpy ( szArgument6, "--clientfeedback" );
            szArguments [5] = szArgument6;
            szArguments [6] = reinterpret_cast < char* > ( CServer::AddServerOutput );

            // We're now running the server
            m_CriticalSection.Unlock ();

            // Call it and grab what it returned
            int iReturn = pfnEntryPoint ( 7, szArguments );

            m_iLastError = iReturn;

            // Lock again
            m_CriticalSection.Lock ();

            // Delete the library
            delete m_pLibrary;
            m_pLibrary = NULL;

            // Return what the server returned
            m_CriticalSection.Unlock ();
            g_bIsStarted = false;
            return iReturn;
        }
    }

    // Delete the library again
    delete m_pLibrary;
    m_pLibrary = NULL;

    // Unlock the critialsection and return failed
    m_CriticalSection.Unlock ();
    g_bIsStarted = false;
    return 1;
}


void CServer::AddServerOutput ( const char* szOutput )
{
    // Make sure the client doesn't process the queue right now
    m_OutputCC.Lock ();

    // Add the string to the queue
    m_OutputQueue.push_back ( szOutput );

    // Unlock again
    m_OutputCC.Unlock ();
}
