/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLogger.cpp
*  PURPOSE:     Server logger class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

FILE* CLogger::m_pLogFile = NULL;
bool CLogger::m_bOutputEnabled = true;

#define MAX_STRING_LENGTH 2048
void CLogger::LogPrintf ( const char* szFormat, ... )
{
    // Output enabled?
    if ( m_bOutputEnabled )
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
	    _vsnprintf ( szBuffer + 11, MAX_STRING_LENGTH - 11, szFormat, marker );
        va_end ( marker );

        // Print it in the console
        g_pServerInterface->Printf ( "%s", szBuffer );

        // If we got a file, print it to that too
        if ( m_pLogFile )
        {
            fprintf ( m_pLogFile, "%s", szBuffer );
            fflush ( m_pLogFile );
        }
    }
}


void CLogger::LogPrint ( const char* szText )
{
    // Send it to LogPrintf as a non-format string
    LogPrintf ( "%s", szText );
}


void CLogger::LogPrintfNoStamp ( const char* szFormat, ... )
{
    // Output enabled?
    if ( m_bOutputEnabled )
    {
        // Convert to a printable string
        char szBuffer [MAX_STRING_LENGTH];
        va_list marker;
        va_start ( marker, szFormat );
	    _vsnprintf ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
        va_end ( marker );

        // Print it in the console
        g_pServerInterface->Printf ( "%s", szBuffer );

        // If we got a file, print it to that too
        if ( m_pLogFile )
        {
            fprintf ( m_pLogFile, "%s", szBuffer );
            fflush ( m_pLogFile );
        }
    }
}


void CLogger::LogPrintNoStamp ( const char* szText )
{
    LogPrintfNoStamp ( "%s", szText );
}


void CLogger::ErrorPrintf ( const char* szFormat, ... )
{
    // Output enabled?
    if ( m_bOutputEnabled )
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
        _vsnprintf ( szBuffer + 18, MAX_STRING_LENGTH - 18, szFormat, marker );
        va_end ( marker );

        // Print it to the console
        g_pServerInterface->Printf ( "%s", szBuffer );

        // If we got a file, print it to that too
        if ( m_pLogFile )   
        {
            fprintf ( m_pLogFile, "%s", szBuffer );
            fflush ( m_pLogFile );
        }
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
        _vsnprintf ( szBuffer + 18, MAX_STRING_LENGTH - 18, szFormat, marker );
        va_end ( marker );

        // Print it to the console
        g_pServerInterface->Printf ( "%s", szBuffer );

        // If we got a file, print it to that too
        if ( m_pLogFile )   
        {
            fprintf ( m_pLogFile, "%s", szBuffer );
            fflush ( m_pLogFile );
        }
    #endif
}


bool CLogger::SetLogFile ( const char* szLogFile )
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
        // Make sure the path to it exists
        MakeSureDirExists ( szLogFile );

        // Create the file
        m_pLogFile = fopen ( szLogFile, "a+" );
        return m_pLogFile != NULL;
    }

    // Return false if we got specified a logfile but weren't able to make one
    return !szLogFile;
}


void CLogger::SetOutputEnabled ( bool bEnabled )
{
    m_bOutputEnabled = bEnabled;
}
