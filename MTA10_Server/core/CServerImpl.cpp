/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerImpl.cpp
*  PURPOSE:     Server class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Oli <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CServerImpl.h"
#include "CCrashHandler.h"
#include "MTAPlatform.h"
#include "ErrorCodes.h"
#include <assert.h>
#include <cstdio>
#include <signal.h>

// Define libraries
char szNetworkLibName[] = "net" MTA_LIB_SUFFIX MTA_LIB_EXTENSION;
char szXMLLibName[] = "xmll" MTA_LIB_SUFFIX MTA_LIB_EXTENSION;

using namespace std;

bool g_bSilent = false;
bool g_bNoTopBar = false;
#ifndef WIN32
bool g_bDaemonized = false;
#endif

#ifdef WIN32
CServerImpl::CServerImpl ( CThreadCommandQueue* pThreadCommandQueue )
#else
CServerImpl::CServerImpl ( void )
#endif
{
    #ifdef WIN32
        m_pThreadCommandQueue = pThreadCommandQueue;
        m_fClientFeedback = NULL;
        m_hConsole = NULL;
    #else
        m_wndMenu = NULL;
        m_wndInput = NULL;
    #endif

    // Init our crashhandler
    CCrashHandler::Init ();

    // Init
    m_pNetwork = NULL;
    m_bRequestedQuit = false;
    m_bRequestedReset = false;
    memset(&m_szInputBuffer, 0, sizeof ( m_szInputBuffer ) * sizeof ( char ) );
    memset(&m_szTag, 0, sizeof ( m_szTag ) * sizeof ( char ) );
    m_uiInputCount = 0;

    // Create the TCP interface
    m_pTCP = new CTCPImpl;
    if ( !m_pTCP->Initialize () )
    {
        Print ( "WARNING: Initializing TCP failed ('%s')\n", m_pTCP->GetLastError () );
    }

    // Create our stuff
    m_pModManager = new CModManagerImpl ( this );
}


CServerImpl::~CServerImpl ( void )
{
    // Destroy our stuff
    delete m_pModManager;
    delete m_pTCP;
}


CNetServer* CServerImpl::GetNetwork ( void )
{
    return m_pNetwork;
}


CModManager* CServerImpl::GetModManager ( void )
{
    return m_pModManager;
}


CTCP* CServerImpl::GetTCP ( void )
{
    return m_pTCP;
}


CXML* CServerImpl::GetXML ( void )
{
    return m_pXML;
}


const char* CServerImpl::GetAbsolutePath ( const char* szRelative, char* szBuffer, unsigned int uiBufferSize )
{
    szBuffer [uiBufferSize-1] = 0;
    _snprintf ( szBuffer, uiBufferSize - 1, "%s/%s", m_strServerPath.c_str (), szRelative );
    return szBuffer;
}


void CServerImpl::Printf ( const char* szFormat, ... )
{
    va_list ap;
    va_start ( ap, szFormat );

    if ( !g_bSilent )
    {
#ifdef WIN32
        vprintf ( szFormat, ap );
#else
        vwprintw ( stdscr, szFormat, ap );
#endif
    }

    // Eventually feed stuff back to our client if we run inside GTA
    #ifdef WIN32
    if ( m_fClientFeedback )
    {
        char szOutput [512];
        szOutput [511] = 0;
        _VSNPRINTF ( szOutput, 511, szFormat, ap );
        m_fClientFeedback ( szOutput );
    }
    #endif

    va_end ( ap );
}

#ifndef WIN32
void CServerImpl::Daemonize () const
{
    if ( fork () ) exit ( 0 );

    close ( 0 );
    assert ( open ( "/dev/null", O_RDONLY ) == 0 );

    close ( 1 );
    assert ( open ( "/dev/null", O_WRONLY ) == 1 );

    close ( 2 );
    assert ( open ( "/dev/null", O_WRONLY ) == 2 );
}
#endif


int CServerImpl::Run ( int iArgumentCount, char* szArguments [] )
{
    // Parse our arguments
    if ( !ParseArguments ( iArgumentCount, szArguments ) )
    {
        return 1;
    }

#ifndef WIN32
    // Daemonize?
    if ( g_bDaemonized )
        Daemonize ();
#endif

    if ( !g_bSilent )
    {
    // Initialize the console handlers
#ifdef WIN32
        // Get the console handle
        m_hConsole = GetStdHandle ( STD_OUTPUT_HANDLE );

        // Enable the default grey color with a black background
        SetConsoleTextAttribute ( m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );

        // Get the console's width
        CONSOLE_SCREEN_BUFFER_INFO ScrnBufferInfo;
        GetConsoleScreenBufferInfo( m_hConsole, &ScrnBufferInfo );

        // Adjust the console's screenbuffer so we can disable a bar at the top
        if ( !g_bNoTopBar )
            ScrnBufferInfo.dwSize.Y = ScrnBufferInfo.srWindow.Bottom + 1;

        SetConsoleWindowInfo ( m_hConsole, TRUE, &ScrnBufferInfo.srWindow );
        SetConsoleScreenBufferSize( m_hConsole, ScrnBufferInfo.dwSize );
#else
        // Initialize the window and any necessary curses options
        initscr ( );
        keypad ( stdscr, TRUE );
        nonl ( );
        cbreak ( );
        noecho ( );
        idlok ( stdscr, FALSE );
        scrollok ( stdscr, TRUE );
        if ( !g_bNoTopBar )
            setscrreg ( 1, LINES - 1 );
        else
            setscrreg ( 0, LINES - 1 );

        // Initialize the colors
        if ( has_colors ( ) )
        {
            start_color ( );

            init_pair ( 1, COLOR_BLACK, COLOR_WHITE );
            init_pair ( 2, COLOR_BLACK, COLOR_GREEN );
            init_pair ( 3, COLOR_WHITE, COLOR_WHITE );
            init_pair ( 4, COLOR_RED, COLOR_WHITE );
            init_pair ( 5, COLOR_GREEN, COLOR_WHITE );
            init_pair ( 6, COLOR_BLUE, COLOR_WHITE );
        }
        // Create the input window
        m_wndInput = subwin ( stdscr, 1, COLS, LINES - 1, 0 );
        wbkgd ( m_wndInput, COLOR_PAIR ( 2 ) );

        // Create the menu window
        if ( !g_bNoTopBar )
        {
            m_wndMenu = subwin ( stdscr, 1, COLS, 0, 0 );
            wbkgd ( m_wndMenu, COLOR_PAIR ( 1 ) );
        }

        // Position the cursor and refresh the physical screen
        if ( !g_bNoTopBar )
            move ( 1, 0 );
        else
            move ( 0, 0 );
        refresh ( );

        // Set our STDIN to non-blocking, if we're on POSIX
        int flags;
        flags = fcntl(0, F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(0, F_SETFL, flags);
#endif
    }

    // Did we find the path? If not, assume our current
    if ( m_strServerPath == "" )
    {
        char szBuffer[ MAX_PATH ];
        getcwd ( szBuffer, MAX_PATH - 1 );
        m_strServerPath = szBuffer;
    }

    // Convert all backslashes to forward slashes
    m_strServerPath = m_strServerPath.Replace ( "\\", "/" );

    // Make sure it has no trailing slash
    m_strServerPath = m_strServerPath.TrimEnd ( "/" );

    // Set the mod path
    m_strServerModPath = m_strServerPath + "/mods/deathmatch";

    // Tell the mod manager the server path
    m_pModManager->SetServerPath ( m_strServerPath );

    // Welcome text
    if ( !g_bSilent )
        Print ( "MTA:BLUE Server for MTA:SA\r\n\r\n" );

    // Load the network DLL
    char szBuffer [MAX_PATH];
    if ( m_NetworkLibrary.Load ( GetAbsolutePath ( szNetworkLibName, szBuffer, MAX_PATH ) ) )
    {
        // Network module compatibility check
        typedef unsigned long (*PFNCHECKCOMPATIBILITY) ( unsigned long );
    PFNCHECKCOMPATIBILITY pfnCheckCompatibility = reinterpret_cast< PFNCHECKCOMPATIBILITY > ( m_NetworkLibrary.GetProcedureAddress ( "CheckCompatibility" ) );
        if ( !pfnCheckCompatibility || !pfnCheckCompatibility ( MTA_DM_SERVER_NET_MODULE_VERSION ) )
        {
            // net.dll doesn't like our version number
            Print ( "Network module not compatible!\n" );
            Print ( "Press Q to shut down the server!\n" );
            Print ( "\n\n\n(If this is a custom build,\n" );
            Print ( " check MTASA_VERSION_TYPE in version.h is set correctly)\n" );
            WaitForKey ( 'q' );
            DestroyWindow ( );
            return ERROR_NETWORK_LIBRARY_FAILED;
        }

        if ( m_XMLLibrary.Load ( GetAbsolutePath ( szXMLLibName, szBuffer, MAX_PATH ) ) )
        {
            // Grab the network interface
            InitNetServerInterface pfnInitNetServerInterface = (InitNetServerInterface) ( m_NetworkLibrary.GetProcedureAddress ( "InitNetServerInterface" ) );
            InitXMLInterface pfnInitXMLInterface = (InitXMLInterface) ( m_XMLLibrary.GetProcedureAddress ( "InitXMLInterface" ) );
            if ( pfnInitNetServerInterface && pfnInitXMLInterface )
            {
                // Call it to grab the network interface class
                m_pNetwork = pfnInitNetServerInterface ();
                m_pXML = pfnInitXMLInterface ();
                if ( m_pNetwork && m_pXML )
                {
                    // Make the modmanager load our mod
                    if ( m_pModManager->Load ( "deathmatch", iArgumentCount, szArguments ) )   // Hardcoded for now
                    {
                        // Enter our mainloop
                        MainLoop ();
                    }
                    else
                    {
                        // Couldn't load our mod
                        Print ( "Press Q to shut down the server!\n" );
                        WaitForKey ( 'q' );
                        DestroyWindow ( );
                        return ERROR_LOADING_MOD;
                    }
                }
                else
                {
                    // Couldn't find the InitNetServerInterface func
                    Print ( "ERROR: Initialization functions failed!\n" );
                    Print ( "Press Q to shut down the server!\n" );
                    WaitForKey ( 'q' );
                    DestroyWindow ( );
                    return ERROR_NETWORK_LIBRARY_FAILED;
                }
            }
            else
            {
                // Couldn't find the InitNetServerInterface func
                Print ( "ERROR: No suitable initialization functions found!\n" );
                Print ( "Press Q to shut down the server!\n" );
                WaitForKey ( 'q' );
                DestroyWindow ( );
                return ERROR_NETWORK_LIBRARY_FAILED;
            }
        }
        else
        {
            // Couldn't load it
            Print ( "ERROR: Loading XML library (%s) failed!\n", szXMLLibName );
            Print ( "Press Q to shut down the server!\n" );
            WaitForKey ( 'q' );
            DestroyWindow ( );
            return ERROR_NO_NETWORK_LIBRARY;
        }
    }
    else
    {
        // Couldn't load it
        Print ( "ERROR: Loading network library (%s) failed!\n", szNetworkLibName );
        Print ( "Press Q to shut down the server!\n" );
        WaitForKey ( 'q' );
        DestroyWindow ( );
        return ERROR_NO_NETWORK_LIBRARY;
    }

    // Normal termination
    DestroyWindow ( );

    // If a reset was requested, tell the main that
    if ( m_bRequestedReset )
    {
        m_bRequestedReset = false;
        m_bRequestedQuit = false;
        return SERVER_RESET_RETURN;
    }

    // Otherwize return no error
    return ERROR_NO_ERROR;
}


void CServerImpl::MainLoop ( void )
{
    // Loop until a termination is requested
    while ( !m_bRequestedQuit )
    {
#ifndef WIN32
        if ( !g_bSilent )
        {
            // Update all the windows, and the physical screen in one burst
            if ( m_wndMenu )
                wnoutrefresh ( m_wndMenu );
            wnoutrefresh ( m_wndInput );
            doupdate ( );
            wbkgd ( m_wndInput, COLOR_PAIR ( 2 ) );
        }
#endif
        if ( !g_bSilent && !g_bNoTopBar )
        {
            // Show the info tag, 80 is a fixed length
            char szInfoTag[80] = { '\0' };
            m_pModManager->GetTag ( &szInfoTag[0], 80 );
            ShowInfoTag ( szInfoTag );
        }

        // Handle the interpreter input
        HandleInput ( );

        // Handle input from the secondary thread
        #ifdef WIN32
        m_pThreadCommandQueue->Process ( m_bRequestedQuit, m_pModManager );
        #endif

        // Pulse the modmanager
        m_pModManager->DoPulse ( );

        if ( m_pModManager->IsFinished () )
        {
            m_bRequestedQuit = true;
        }

        // Limit the pulses to avoid heavy CPU usage
        Sleep ( 10 );
    }

    // Unload the current mod
    m_pModManager->Unload ();
}

/*************************/
/* Tag color interpreter */
/* --------------------- */
/* 128 | white           */
/* 129 | grey            */
/* 130 | red             */
/* 131 | green           */
/* 132 | blue            */
/* 133 | light red       */
/* 134 | light green     */
/* 135 | light blue      */
/*************************/
void CServerImpl::ShowInfoTag ( char* szTag )
{
    if ( g_bSilent || g_bNoTopBar )
        return;
#ifdef WIN32
    // Windows console code
        // Get the console's width
        CONSOLE_SCREEN_BUFFER_INFO ScrnBufferInfo;
        GetConsoleScreenBufferInfo( m_hConsole, &ScrnBufferInfo );

        COORD BufferSize = { ScrnBufferInfo.dwSize.X, 1 };
        COORD TopLeft = { 0, ScrnBufferInfo.srWindow.Top };
        SMALL_RECT Region = { 0, ScrnBufferInfo.srWindow.Top, ScrnBufferInfo.dwSize.X, 1 };

        // If the screenbuffer doesn't exist yet, or if the tag is changed
        if ( m_ScrnBuffer == NULL || strcmp ( szTag, m_szTag ) )
        {
            int ScrnBufferCount = 0;
            strcpy(m_szTag, szTag);

            // Construct the screenbuffer
            for ( int i = 0 ; i < ScrnBufferInfo.dwSize.X ; i++ )
            {
                if ( szTag[i] == NULL )
                {
                    // No more tag data, so fill it up with spaces and break the loop
                    for ( int j = ScrnBufferCount ; j < ScrnBufferInfo.dwSize.X ; j++ )
                    {
                        m_ScrnBuffer[j].Char.AsciiChar = ' ';
                        m_ScrnBuffer[j].Attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
                    }
                    break;
                } else {
                    // The color interpreter
                    switch ( ( unsigned char ) ( szTag[i] ) )
                    {
                        case 128: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
                        case 129: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
                        case 130: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_RED; break;
                        case 131: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_GREEN; break;
                        case 132: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_BLUE; break;
                        case 133: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
                        case 134: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
                        case 135: m_ScrnBuffer[ScrnBufferCount].Attributes = FOREGROUND_INTENSITY | FOREGROUND_BLUE; break;
                        default: m_ScrnBuffer[ScrnBufferCount].Attributes = 0; break;
                    }

                    if ( (unsigned char)szTag[i] > 127 ) {
                        // If this is a color code, skip to the next character, so we can color that one
                        i++;
                    }
                    m_ScrnBuffer[ScrnBufferCount].Char.AsciiChar = szTag[i];

                    // Enable a grey background
                    m_ScrnBuffer[ScrnBufferCount++].Attributes |= ( BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE );
                }
            }
        }
        WriteConsoleOutput ( m_hConsole, m_ScrnBuffer, BufferSize, TopLeft, &Region);
#else
    // Linux curses variant, so much easier :)
    int iAttr = COLOR_PAIR ( 1 );

    // Position the cursor
    wmove ( m_wndMenu, 0, 0 );

    for ( int i = 0 ; i < COLS ; i++ )
    {
        // Break if we reached szTag's end
        if ( szTag[i] == '\0' )
            break;

        // Apply the attributes
        switch ( ( unsigned char ) ( szTag[i] ) )
        {
            case 128: iAttr = COLOR_PAIR ( 3 ) | A_BOLD; break;
            case 129: iAttr = COLOR_PAIR ( 3 ); break;
            case 130: iAttr = COLOR_PAIR ( 4 ); break;
            case 131: iAttr = COLOR_PAIR ( 5 ); break;
            case 132: iAttr = COLOR_PAIR ( 6 ); break;
            case 133: iAttr = COLOR_PAIR ( 4 ) | A_BOLD; break;
            case 134: iAttr = COLOR_PAIR ( 5 ) | A_BOLD; break;
            case 135: iAttr = COLOR_PAIR ( 6 ) | A_BOLD; break;
            default:
                waddch ( m_wndMenu, szTag[i] | iAttr );
                iAttr = COLOR_PAIR ( 1 );
            break;
        }
    }
#endif
}

void CServerImpl::HandleInput ( void )
{
    int iStdIn = 0;

    // Get the STDIN input
#ifdef WIN32
    if ( kbhit () )
    {
        iStdIn = getch();
    }
#else
    iStdIn = getch();
    if ( iStdIn == ERR)
        iStdIn = 0;
#endif

    if ( iStdIn == 0 )
        return;

    // Add the character to the buffer
    if ( m_uiInputCount >= sizeof ( m_szInputBuffer ) )
    {
        memset(&m_szInputBuffer, 0, sizeof ( m_szInputBuffer ) );
        m_uiInputCount = 0;
    }

    switch ( iStdIn )
    {
        case '\n':  // Newlines and carriage returns
        case '\r':
#ifdef WIN32
            // Echo a newline
            Printf ( "\n" );
#else
            if ( !g_bSilent )
            {
                // Clear the input window
                wclear ( m_wndInput );
                printw ( "%s\n", m_szInputBuffer );
            }
#endif

            if ( m_uiInputCount > 0 )
            {
                // Check for the most important command: quit
                if ( !stricmp ( m_szInputBuffer, "quit" ) || !stricmp ( m_szInputBuffer, "exit" ) )
                {
                    m_bRequestedQuit = true;
                }
                else if ( !stricmp ( m_szInputBuffer, "reset" ) )
                {
                    m_bRequestedReset = true;
                    m_bRequestedQuit = true;
                }
                else
                {
                    // Otherwise, pass the command to the mod's input handler
                    m_pModManager->HandleInput ( m_szInputBuffer );
                }
            }

            memset(&m_szInputBuffer, 0, sizeof ( m_szInputBuffer ) );
            m_uiInputCount = 0;
        break;

        case KEY_BACKSPACE: // Backspace
            // Insert a blank space + backspace
#ifdef WIN32
            Printf ( "%c %c", 0x08, 0x08 );
#else
            if ( !g_bSilent )
                wprintw ( m_wndInput, "%c %c", 0x08, 0x08 );
#endif
            m_uiInputCount--;
            m_szInputBuffer[m_uiInputCount] = 0;
        break;

#ifdef WIN32    // WIN32: we have to use a prefix code, this routine opens an extra switch
        case KEY_EXTENDED:
            // Color the text
            if ( !g_bSilent )
                SetConsoleTextAttribute ( m_hConsole, FOREGROUND_GREEN | FOREGROUND_RED );
            if ( kbhit () )
            {
                iStdIn = getch();
            }
            switch ( iStdIn )
            {
#endif

        case KEY_LEFT:
        {
            char szBuffer [255];
            memset ( szBuffer, 0, sizeof ( szBuffer ) );

            if ( m_uiInputCount > 0 )
            {
                m_uiInputCount--;
            }
            strncpy ( &szBuffer[0], &m_szInputBuffer[0], m_uiInputCount );
            szBuffer[m_uiInputCount] = 0;
#ifdef WIN32
            Printf ( "\r%s", szBuffer );
#else
            if ( !g_bSilent )
                wprintw ( m_wndInput, "\r%s", szBuffer );
#endif
            break;
        }

        case KEY_RIGHT:
        {
            char szBuffer [255];
            memset ( szBuffer, 0, sizeof ( szBuffer ) );

            if ( m_uiInputCount < strlen ( m_szInputBuffer ) )
            {
                m_uiInputCount++;
            }
            strncpy ( &szBuffer[0], &m_szInputBuffer[0], m_uiInputCount );
            szBuffer[m_uiInputCount] = 0;
#ifdef WIN32
            Printf ( "\r%s", szBuffer );
#else
            if ( !g_bSilent )
                wprintw ( m_wndInput, "\r%s", szBuffer );
#endif
            break;
        }

        case KEY_UP:    // Up-arrow cursor
        break;

        case KEY_DOWN:  // Down-arrow cursor
        break;

#ifdef WIN32    // WIN32: Close the switch again
            }
            // Restore the color
            if ( !g_bSilent )
                SetConsoleTextAttribute ( m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );

        break;  // KEY_EXTENDED
#endif

        default:
#ifdef WIN32
            // Color the text
            if ( !g_bSilent )
                SetConsoleTextAttribute ( m_hConsole, FOREGROUND_GREEN | FOREGROUND_RED );

            // Echo the input
            Printf ( "%c", iStdIn );
#else
            if ( !g_bSilent )
                wprintw ( m_wndInput, "%c", iStdIn );
#endif

            m_szInputBuffer[m_uiInputCount++] = iStdIn;

#ifdef WIN32
            // Restore the color
            if ( !g_bSilent )
                SetConsoleTextAttribute ( m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
#endif
        break;
    }
}


bool CServerImpl::ParseArguments ( int iArgumentCount, char* szArguments [] )
{
#ifndef WIN32
    // Default to a simple console if running under 'nohup'
    struct sigaction sa;
    sigaction ( SIGHUP, NULL, &sa );
    if ( sa.sa_handler == SIG_IGN )
        g_bNoTopBar = true;
#endif

    // Iterate our arguments
    unsigned char ucNext = 0;
    for ( int i = 0; i < iArgumentCount; i++ )
    {
        switch ( ucNext )
        {
            // Run from path?
            case 'D':
            {
                // Set it as our current path.
                m_strServerPath = szArguments [i];
                ucNext = 0;
                break;
            }

            // Client feedback pointer?
            #ifdef WIN32
            case 'c':
            {
                m_fClientFeedback = reinterpret_cast < FClientFeedback* > ( szArguments [i] );
                ucNext = 0;
                break;
            }
            #endif


            // Nothing we know, proceed
            default:
            {
                if ( strcmp ( szArguments [i], "-D" ) == 0 )
                {
                    ucNext = 'D';
                }
                else if ( strcmp ( szArguments [i], "-s" ) == 0 )
                {
                    g_bSilent = true;
                }
#ifndef WIN32
                else if ( strcmp ( szArguments [i], "-d" ) == 0 )
                {
                    g_bDaemonized = true;
                }
#endif
                else if ( strcmp ( szArguments [i], "-t" ) == 0 )
                {
                    g_bNoTopBar = true;
                }
                else if ( strcmp ( szArguments [i], "-f" ) == 0 )
                {
                    g_bNoTopBar = false;
                }

                #ifdef WIN32
                else if ( strcmp ( szArguments [i], "--clientfeedback" ) == 0 )
                {
                    ucNext = 'c';
                }
                #endif
            }
        }
    }


    return true;
}


bool CServerImpl::IsKeyPressed ( int iKey )
{
    // Is the key pressed?
#ifdef WIN32
    if ( kbhit () )
    {
        return getch () == iKey;
    }
#else
    refresh ();
    return getchar () == iKey;
#endif

    // Not pressed
    return false;
}

void CServerImpl::DestroyWindow ( void )
{
#ifndef WIN32
    if ( !g_bSilent )
    {
        if ( m_wndMenu )
            delwin ( m_wndMenu );
        delwin ( m_wndInput );
        endwin ( );
    }
#endif
}

void CServerImpl::WaitForKey ( int iKey )
{
    if ( !g_bSilent )
    {
        for ( ;; )
        {
            // Is the key pressed?
            if ( IsKeyPressed ( iKey ) )
            {
                return;
            }

            // Limit the looping a little to prevent heavy CPU usage
            SleepMs ( 10 );
        }
    }
}


void CServerImpl::SleepMs ( unsigned long ulMs )
{
    Sleep ( ulMs );
}
