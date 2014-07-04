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
FILE* CLogger::m_pAuthFile = NULL;
bool CLogger::m_bOutputEnabled = true;

#define MAX_STRING_LENGTH 2048
void CLogger::LogPrintf ( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Timestamp and send to the console and logfile
    HandleLogPrint ( true, "", szBuffer, true, true, false );
}


void CLogger::LogPrint ( const char* szText )
{
    // Timestamp and send to the console and logfile
    HandleLogPrint ( true, "", szText, true, true, false );
}


void CLogger::LogPrintfNoStamp ( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Send to the console and logfile
    HandleLogPrint ( false, "", szBuffer, true, true, false );
}


void CLogger::LogPrintNoStamp ( const char* szText )
{
    // Send to the console and logfile
    HandleLogPrint ( false, "", szText, true, true, false );
}


void CLogger::ErrorPrintf ( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Timestamp and send to the console and logfile
    HandleLogPrint ( true, "ERROR: ", szBuffer, true, true, false );
}


void CLogger::DebugPrintf ( const char* szFormat, ... )
{
    #ifdef MTA_DEBUG
        // Compose the formatted message
        char szBuffer [MAX_STRING_LENGTH];
        va_list marker;
        va_start ( marker, szFormat );
        _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
        va_end ( marker );

        // Timestamp and send to the console and logfile
        HandleLogPrint ( true, "DEBUG: ", szBuffer, true, true, false );
    #endif
}


void CLogger::AuthPrintf ( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Timestamp and send to the console, logfile and authfile
    HandleLogPrint ( true, "", szBuffer, true, true, true );
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
    if ( szLogFile && szLogFile[0] )
    {
        // Make sure the path to it exists
        MakeSureDirExists ( szLogFile );

        // Create the file
        m_pLogFile = fopen ( szLogFile, "a+" );
        return m_pLogFile != NULL;
    }

    // Return true if supplied file name was empty
    return true;
}


bool CLogger::SetAuthFile ( const char* szAuthFile )
{
    // Eventually delete our current file
    if ( m_pAuthFile )
    {
        fclose ( m_pAuthFile );
        m_pAuthFile = NULL;
    }

    // Eventually open a new file
    if ( szAuthFile && szAuthFile[0] )
    {
        // Make sure the path to it exists
        MakeSureDirExists ( szAuthFile );

        // Create the file
        m_pAuthFile = fopen ( szAuthFile, "a+" );
        return m_pAuthFile != NULL;
    }

    // Return true if supplied file name was empty
    return true;
}


void CLogger::SetOutputEnabled ( bool bEnabled )
{
    m_bOutputEnabled = bEnabled;
}


// Handle where to send the message
void CLogger::HandleLogPrint ( bool bTimeStamp, const char* szPrePend, const char* szMessage, bool bToConsole, bool bToLogFile, bool bToAuthFile )
{
    if ( !m_bOutputEnabled )
        return;

    // Put the timestamp at the beginning of the string
    string strOutputShort;
    string strOutputLong;
    if ( bTimeStamp )
    {
        char szBuffer [MAX_STRING_LENGTH] = { "\0" };
        time_t timeNow;
        time ( &timeNow );
        tm* pCurrentTime = localtime ( &timeNow );
        if ( !strftime ( szBuffer, MAX_STRING_LENGTH - 1, "[%H:%M:%S] ", pCurrentTime ) )
            szBuffer[0] = 0;
        strOutputShort = szBuffer;
        if ( !strftime ( szBuffer, MAX_STRING_LENGTH - 1, "[%Y-%m-%d %H:%M:%S] ", pCurrentTime ) )
            szBuffer[0] = 0;
        strOutputLong = szBuffer;
    }

    // Build the final string
    strOutputShort = strOutputShort + szPrePend + szMessage;
    strOutputLong = strOutputLong + szPrePend + szMessage;

    // Maybe print it in the console
    if ( bToConsole )
        g_pServerInterface->Printf ( "%s", strOutputShort.c_str () );

    // Maybe print it to the log file
    if ( bToLogFile && m_pLogFile )
    {
        fprintf ( m_pLogFile, "%s", strOutputLong.c_str () );
        fflush ( m_pLogFile );
    }

    // Maybe print it to the auth file
    if ( bToAuthFile && m_pAuthFile )
    {
        fprintf ( m_pAuthFile, "%s", strOutputLong.c_str () );
        fflush ( m_pAuthFile );
    }
}
