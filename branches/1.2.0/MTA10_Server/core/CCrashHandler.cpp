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

#ifdef WIN32
#include <tchar.h>
#include <dbghelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                    );

#endif

void CCrashHandler::Init ( void )
{
    // Set a global filter
    #ifdef WIN32
    SetCrashHandlerFilter ( HandleExceptionGlobal );
    #endif
}

#ifdef WIN32

long WINAPI CCrashHandler::HandleExceptionGlobal ( _EXCEPTION_POINTERS* pException )
{
    // Write the dump, run error tool and end
    DumpMiniDump ( pException );
    RunErrorTool ();
    return EXCEPTION_CONTINUE_SEARCH;
}


void CCrashHandler::DumpMiniDump ( _EXCEPTION_POINTERS* pException )
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
            CreateDirectory ( "dumps", 0 );

            // Add a log entry.
            char szFilename [256];
            sprintf ( szFilename, "dumps/server_%s_%04d%02d%02d_%02d%02d.dmp", MTA_DM_BUILDTAG_LONG,
                                                                               SystemTime.wYear,
                                                                               SystemTime.wMonth,
                                                                               SystemTime.wDay,
                                                                               SystemTime.wHour,
                                                                               SystemTime.wMinute );

            // Create the file
            HANDLE hFile = CreateFile ( szFilename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
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

