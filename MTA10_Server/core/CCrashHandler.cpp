/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCrashHandler.cpp
*  PURPOSE:     Crash handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CCrashHandler.h"
#include <string>
#ifndef WIN32
    #include <execinfo.h>
#endif

#ifndef WIN32
    #include <ncursesw/curses.h>
    extern "C" WINDOW* m_wndMenu;
    extern "C" WINDOW* m_wndInput;
    extern "C" bool g_bNoCurses;
    #include <client/linux/handler/exception_handler.h>
    bool DumpCallback( const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded );
    static SString ms_strDumpPathFilename;
#endif
static SString ms_strDumpPath;

#ifdef WIN32
#include <ctime>
#include <tchar.h>
#include <dbghelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                    );

#endif

void CCrashHandler::Init ( const SString& strInServerPath )
{
    SString strServerPath = strInServerPath;
    if ( strServerPath == "" )
        strServerPath = GetSystemCurrentDirectory();
    ms_strDumpPath = PathJoin( strServerPath, SERVER_DUMP_PATH );

    // Set a global filter
    #ifdef WIN32
        SetCrashHandlerFilter ( HandleExceptionGlobal );
    #else
        // Prepare initial dumpfile name
        time_t pTime = time( NULL );
        struct tm* tm = localtime( &pTime );
        SString strFilename( "server_%s_%04d%02d%02d_%02d%02d.dmp",
                                        MTA_DM_BUILDTAG_LONG,
                                        tm->tm_year + 1900,
                                        tm->tm_mon + 1,
                                        tm->tm_mday,
                                        tm->tm_hour,
                                        tm->tm_min
                                    );
        ms_strDumpPathFilename = PathJoin( ms_strDumpPath, strFilename );
        MakeSureDirExists( ms_strDumpPathFilename );

        #ifdef WITH_BACKTRACE_ONLY
            signal ( SIGSEGV, HandleExceptionGlobal );
        #else
            google_breakpad::MinidumpDescriptor descriptor( ms_strDumpPath );
            static google_breakpad::ExceptionHandler eh( descriptor, NULL, DumpCallback, NULL, true, -1 );
        #endif
    #endif
}

#ifndef WIN32

// Save basic backtrace info into a file. Forced inline to avoid backtrace pollution
inline __attribute__((always_inline))
static void SaveBacktraceSummary()
{
    // Collect backtrace information
    void * buffer [ 100 ];
    int iAmount = backtrace ( buffer, sizeof buffer );
    iAmount = Min < int > ( iAmount, NUMELMS( buffer ) );
    char ** symbols = backtrace_symbols ( buffer, iAmount );

    // Generate a .log file
    time_t pTime = time ( NULL );
    struct tm * tm = localtime ( &pTime );

    SString sFileName;
    sFileName.Format ( "server_%s_%04d%02d%02d_%02d%02d.log", MTA_DM_BUILDTYPE, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min );

    SString sContent;
    sContent += SString ( "MTA:SA Server v%s-r%d-%s crash report.\n", MTA_DM_VERSIONSTRING, MTASA_VERSION_BUILD, MTA_DM_BUILDTYPE );
    sContent += SString ( "%04d-%02d-%02d %02d:%02d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min );
    sContent += SString ( "Caught %d addresses ...\n\n", iAmount );
    sContent += "Backtrace:\n";

    for ( int i = 0; i < iAmount; i++ )
    {
        if ( symbols [ i ] )
        {
            sContent += SString ( "#%d - %s\n", i, symbols [ i ] );
        }
    }
    sContent += std::string( 80, '-' ) + "\n";

    // Write the content to the file and close
    MakeSureDirExists ( PathJoin( ms_strDumpPath, sFileName ) );
    FileAppend( PathJoin( ms_strDumpPath, sFileName ), sContent );
    FileAppend( PathJoin( ms_strDumpPath, "server_pending_upload.log" ), sContent );

    free ( symbols );

    // Try to close window gracefully 
    if ( !g_bSilent && !g_bNoCurses && m_wndInput )
    {
        if ( m_wndMenu )
        {
            delwin ( m_wndMenu );
            m_wndMenu = NULL;
        }
        delwin ( m_wndInput );
        m_wndInput = NULL;
        endwin ( );
    }
}


// Linux crash callback when using google-breakpad
bool DumpCallback( const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded )
{
    // Set inital dump file name (Safeish)
    rename( descriptor.path(), ms_strDumpPathFilename );

    // Set final dump file name (Not so safe)
    time_t pTime = time( NULL );
    struct tm* tm = localtime( &pTime );
    SString strFilename( "server_%s_%04d%02d%02d_%02d%02d.dmp",
                                    MTA_DM_BUILDTAG_LONG,
                                    tm->tm_year + 1900,
                                    tm->tm_mon + 1,
                                    tm->tm_mday,
                                    tm->tm_hour,
                                    tm->tm_min
                                );
    SString strFinalDumpPathFilename = PathJoin( ms_strDumpPath, strFilename );
    rename( ms_strDumpPathFilename, strFinalDumpPathFilename );

    SaveBacktraceSummary();
    FileSave( PathJoin( ms_strDumpPath, "server_pending_upload_filename" ), strFinalDumpPathFilename );

    // Return false to indicate exception has not been handled (and allow core dump?)
    return false;
}

void CCrashHandler::HandleExceptionGlobal ( int iSig )
{
    SaveBacktraceSummary();
    exit ( EXIT_FAILURE );
}

#else

long WINAPI CCrashHandler::HandleExceptionGlobal ( _EXCEPTION_POINTERS* pException )
{
    // Create the exception information class
    CExceptionInformation_Impl* pExceptionInformation = new CExceptionInformation_Impl;
    pExceptionInformation->Set ( pException->ExceptionRecord->ExceptionCode, pException );

    // Write the dump
    DumpMiniDump ( pException, pExceptionInformation );
    RunErrorTool ();
    TerminateProcess ( GetCurrentProcess (), 1 );
    return EXCEPTION_CONTINUE_SEARCH;
}


void CCrashHandler::DumpMiniDump ( _EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation )
{
    // Try to load the DLL in our directory
    HMODULE hDll = NULL;
    char szDbgHelpPath [MAX_PATH];
    if ( GetModuleFileNameA ( NULL, szDbgHelpPath, MAX_PATH ) )
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
        hDll = LoadLibrary ( "DBGHELP.DLL" );
    }

    // We could load a dll?
    if ( hDll )
    {
        // Grab the MiniDumpWriteDump proc address
        MINIDUMPWRITEDUMP pDump = reinterpret_cast < MINIDUMPWRITEDUMP > ( GetProcAddress( hDll, "MiniDumpWriteDump" ) );
        if ( pDump )
        {
            // Grab the current time
            // Ask windows for the system time.
            SYSTEMTIME SystemTime;
            GetLocalTime ( &SystemTime );

            // Create the dump directory
            CreateDirectory ( ms_strDumpPath, 0 );
            CreateDirectory ( PathJoin( ms_strDumpPath, "private" ), 0 );

            SString strModuleName = pExceptionInformation->GetModuleBaseName ();
            strModuleName = strModuleName.ReplaceI ( ".dll", "" ).Replace ( ".exe", "" ).Replace ( "_", "" ).Replace ( ".", "" ).Replace ( "-", "" );
            if ( strModuleName.length () == 0 )
                strModuleName = "unknown";

            SString strFilename ( "server_%s_%s_%08x_%x_%04d%02d%02d_%02d%02d.dmp",
                                         MTA_DM_BUILDTAG_LONG,
                                         strModuleName.c_str (),
                                         pExceptionInformation->GetAddressModuleOffset (),
                                         pExceptionInformation->GetCode () & 0xffff,
                                         SystemTime.wYear,
                                         SystemTime.wMonth,
                                         SystemTime.wDay,
                                         SystemTime.wHour,
                                         SystemTime.wMinute
                                       );

            SString strFinalDumpPathFilename = PathJoin( ms_strDumpPath, "private", strFilename );

            // Create the file
            HANDLE hFile = CreateFile ( strFinalDumpPathFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            if ( hFile != INVALID_HANDLE_VALUE )
            {
                // Create an exception information struct
                _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
                ExInfo.ThreadId = GetCurrentThreadId ();
                ExInfo.ExceptionPointers = pException;
                ExInfo.ClientPointers = FALSE;

                // Write the dump
                pDump ( GetCurrentProcess(), GetCurrentProcessId(), hFile, (MINIDUMP_TYPE)( MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory ), &ExInfo, NULL, NULL );

                // Close the dumpfile
                CloseHandle ( hFile );

                FileSave( PathJoin( ms_strDumpPath, "server_pending_upload_filename" ), strFinalDumpPathFilename );
            }

            // Write a log with the generic exception information
            FILE* pFile = fopen ( PathJoin( ms_strDumpPath, "server_pending_upload.log" ), "a+" );
            if ( pFile )
            {
               // Header
                fprintf ( pFile, "%s", "** -- Unhandled exception -- **\n\n" );

                // Write the time
                time_t timeTemp;
                time ( &timeTemp );

                SString strMTAVersionFull = SString ( "%s", MTA_DM_BUILDTAG_LONG );

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
            }
        }

        // Free the DLL again
        FreeLibrary ( hDll );
    }
}


void CCrashHandler::RunErrorTool ( void )
{

}

#endif

