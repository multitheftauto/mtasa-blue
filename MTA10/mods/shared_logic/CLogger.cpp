/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLogger.cpp
*  PURPOSE:     Logger class
*  DEVELOPERS:  Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

FILE* CLogger::m_pLogFile = NULL;

#define MAX_STRING_LENGTH 2048
void CLogger::LogPrintf ( const char* szFormat, ... )
{
    // Put the timestamp at the beginning of the string
    char szBuffer [MAX_STRING_LENGTH];
    time_t timeNow;
    time ( &timeNow );
    tm* pCurrentTime = localtime ( &timeNow );
    strftime ( szBuffer, 32, "[%H:%M:%S] ", pCurrentTime );

    // Put the rest of the string sent to us into the rest of the buffer
    va_list marker;
    va_start ( marker, szFormat );
	//vsnprintf ( szBuffer + 11, MAX_STRING_LENGTH - 11, szFormat, marker );
    va_end ( marker );

    // Print it in the console
    //g_pServerInterface->Printf ( "%s", szBuffer );

    // If we got a file, print it to that too
    if ( m_pLogFile )
    {
        fprintf ( m_pLogFile, "%s\n", szFormat );
        fflush ( m_pLogFile );
    }
}


void CLogger::LogPrint ( const char* szText )
{
    // Send it to LogPrintf as a non-format string
    LogPrintf ( szText );
}


void CLogger::LogPrintfNoStamp ( const char* szFormat, ... )
{
    // Convert to a printable string
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
	//vsnprintf ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Print it in the console
    //g_pServerInterface->Printf ( "%s", szBuffer );

    // If we got a file, print it to that too
    if ( m_pLogFile )
    {
        fprintf ( m_pLogFile, "%s", szBuffer );
        fflush ( m_pLogFile );
    }
}


void CLogger::LogPrintNoStamp ( const char* szText )
{
    LogPrintfNoStamp ( "%s", szText );
}


void CLogger::ErrorPrintf ( const char* szFormat, ... )
{
    // Put the timestamp at the beginning of the string
    char szBuffer [MAX_STRING_LENGTH];
    time_t timeNow;
    time ( &timeNow );
    tm* pCurrentTime = localtime ( &timeNow );
    strftime ( szBuffer, 32, "[%H:%M:%S] ERROR: ", pCurrentTime );

    // Convert it to a string
    va_list marker;
    va_start ( marker, szFormat );
    //vsnprintf ( szBuffer + 18, MAX_STRING_LENGTH - 18, szFormat, marker );
    va_end ( marker );

    // Print it to the console
    //g_pServerInterface->Printf ( "%s", szBuffer );

    // If we got a file, print it to that too
    if ( m_pLogFile )   
    {
        fprintf ( m_pLogFile, "%s", szBuffer );
        fflush ( m_pLogFile );
    }
}


void CLogger::DebugPrintf ( const char* szFormat, ... )
{
    #ifdef MTA_DEBUG
        // Put the timestamp at the beginning of the string
        char szBuffer [MAX_STRING_LENGTH];
        time_t timeNow;
        time ( &timeNow );
        tm* pCurrentTime = localtime ( &timeNow );
        strftime ( szBuffer, 32, "[%H:%M:%S] DEBUG: ", pCurrentTime );

        // Convert it to a string
        va_list marker;
        va_start ( marker, szFormat );
        //vsnprintf ( szBuffer + 18, MAX_STRING_LENGTH - 18, szFormat, marker );
        va_end ( marker );

        // Print it to the console
        //g_pServerInterface->Printf ( "%s", szBuffer );

        // If we got a file, print it to that too
        if ( m_pLogFile )   
        {
            fprintf ( m_pLogFile, "%s", szBuffer );
            fflush ( m_pLogFile );
        }
    #endif
}


void CLogger::SetLogFile ( const char* szLogFile )
{
    // Eventually delete our current file
    if ( m_pLogFile )
    {
        fclose ( m_pLogFile );
        m_pLogFile = NULL;
    }

    // Eventually open a new file
    if ( szLogFile )
    {
        m_pLogFile = fopen ( szLogFile, "a+" );
    }
}
