/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLogger.cpp
*  PURPOSE:     Logging functionality
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

#define MAX_STRING_LENGTH 2048
template<> CLogger * CSingleton< CLogger >::m_pSingleton = NULL;

CLogger::CLogger ( )
{
    // Create the log file.
    File.open ( "logfile.txt", ios::out );
}

CLogger::~CLogger ( )
{
    // Destroy the log file.
    File.close ( );
}

void CLogger::WriteDebugEventInt ( const string& EventDescription ) 
{
    SYSTEMTIME      SystemTime;
    stringstream    s ( stringstream::in | stringstream::out );
    string          ConOut;

    // Output message to debug console
#ifdef MTA_DEBUG
    OutputDebugString ( string ( EventDescription + '\n' ).c_str ( ) );
#endif

    // Ask windows for the system time.
    GetLocalTime ( &SystemTime );

    // Add a log entry.
    s    << "[DEBUG EVENT] " << setw ( 2 )           << setfill ( '0' )
                             << SystemTime.wMonth    << "-" 
                             << setw ( 2 )           << setfill ( '0' )
                             << SystemTime.wDay      << "-"
                             << setw ( 4 )           << setfill ( '0' )
                             << SystemTime.wYear     << " @ "
                             << setw ( 2 )           << setfill ( '0' )
                             << SystemTime.wHour     << ":"
                             << setw ( 2 )           << setfill ( '0' )
                             << SystemTime.wMinute   << ":"
                             << setw ( 2 )           << setfill ( '0' )
                             << SystemTime.wSecond   << " # " 
                             << EventDescription     << endl;
    ConOut = s.str ( );

    File << ConOut;
    File.flush ( );
}

void CLogger::WriteEvent ( const string& EventDescription )
{
    SYSTEMTIME SystemTime;

    // Ask windows for the system time.
    GetLocalTime ( &SystemTime );

    // Add a log entry.
    File << "[EVENT] " << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wMonth    << "-" 
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wDay      << "-"
                       << setw ( 4 )           << setfill ( '0' )
                       << SystemTime.wYear     << " @ "
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wHour     << ":"
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wMinute   << ":"
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wSecond   << " # " 
                       << EventDescription     << endl;
}

void CLogger::OutputDebugMessage ( const string& DebugMessage )
{
    #ifdef MTA_DEBUG
        WriteEvent ( DebugMessage );
        OutputDebugString ( DebugMessage.c_str ( ) );
    #endif
}

void CLogger::WriteErrorEventInt ( const string& ErrorMessage )
{
    // Show user an error message box.
    MessageBox ( 0, ErrorMessage.c_str ( ), "[ERROR EVENT]", MB_ICONEXCLAMATION );

    // Write this event to the event log.
    WriteEvent ( ErrorMessage );
}

void CLogger::ErrorPrintf ( const char* szFormat, ... )
{
    // Put the timestamp at the beginning of the string
    char szBuffer [MAX_STRING_LENGTH];

    // Convert it to a string
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Print it to the console
    WriteEvent ( szBuffer );
}
