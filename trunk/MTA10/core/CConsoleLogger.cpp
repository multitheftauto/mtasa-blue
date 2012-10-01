/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleLogger.cpp
*  PURPOSE:     Console Logging functionality
*  DEVELOPERS:  Derek Abdine <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

#define MAX_STRING_LENGTH 2048
template<> CConsoleLogger * CSingleton< CConsoleLogger >::m_pSingleton = NULL;

CConsoleLogger::CConsoleLogger ( )
{
    // Ask windows for the system time.
    SYSTEMTIME SystemTime;
    GetLocalTime ( &SystemTime );
    
    // Convert date to a string
    char szDate[9] = {'\0'};
    if  ( SystemTime.wMonth < 10 )
    {
        if ( SystemTime.wDay < 10 )
        {
            sprintf ( szDate, "%i0%i0%i", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
        }
        else
        {
            sprintf ( szDate, "%i0%i%i", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
        }
    }
    else if ( SystemTime.wDay < 10 )
    {
        sprintf ( szDate, "%i%i0%i", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
    }
    else
    {
        sprintf ( szDate, "%i%i%i", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay );
    }

    // Create file name
    m_strFilename = CalcMTASAPath ( PathJoin ( "MTA", "logs", "consolelogfile-" ) );
    m_strFilename.append ( szDate );
    m_strFilename.append ( ".log" );

    MakeSureDirExists ( m_strFilename );

    // get the file stream
    File.open ( m_strFilename.c_str(), ios::app );
}


CConsoleLogger::~CConsoleLogger ( )
{
    File.close();
}


void CConsoleLogger::WriteLine ( const string& strLine )
{
    SYSTEMTIME SystemTime;

    // Ask windows for the system time.
    GetLocalTime ( &SystemTime );

    // Output to log
    File               << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wDay      << "-" 
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wMonth    << "-"
                       << setw ( 4 )           << setfill ( '0' )
                       << SystemTime.wYear     << " @ "
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wHour     << ":"
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wMinute   << ":"
                       << setw ( 2 )           << setfill ( '0' )
                       << SystemTime.wSecond   << " # " 
                       << strLine     << endl;
}


void CConsoleLogger::LinePrintf ( const char* szFormat, ... )
{
    char szBuffer [MAX_STRING_LENGTH];

    // Convert it to a string
    va_list marker;
    va_start ( marker, szFormat );
    VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Replace possible LF
    for ( int i = 0; i != MAX_STRING_LENGTH; i++ )
    {
        if ( szBuffer[i] == 10 )
        {
            szBuffer[i] = ' ';
        }
    }

    // Send to output function
    WriteLine ( szBuffer );
}