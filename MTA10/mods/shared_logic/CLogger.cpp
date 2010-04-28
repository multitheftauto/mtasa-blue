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
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Timestamp and send to the console and logfile
    HandleLogPrint ( true, "", szBuffer, true, true );
}


void CLogger::LogPrint ( const char* szText )
{
    // Timestamp and send to the console and logfile
    HandleLogPrint ( true, "", szText, true, true );
}

#if 0   // Currently unused
void CLogger::LogPrintfNoStamp ( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Send to the console and logfile
    HandleLogPrint ( false, "", szBuffer, true, true );
}


void CLogger::LogPrintNoStamp ( const char* szText )
{
    // Send to the console and logfile
    HandleLogPrint ( false, "", szText, true, true );
}
#endif

void CLogger::ErrorPrintf ( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer [MAX_STRING_LENGTH];
    va_list marker;
    va_start ( marker, szFormat );
    _VSNPRINTF ( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end ( marker );

    // Timestamp and send to the console and logfile
    HandleLogPrint ( true, "ERROR: ", szBuffer, true, true );
}

#if 0   // Currently unused
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
        HandleLogPrint ( true, "DEBUG: ", szBuffer, true, true );
    #endif
}
#endif

void CLogger::SetLogFile ( const char* szLogFile )
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
        m_pLogFile = fopen ( szLogFile, "a+" );
    }
}


// Handle where to send the message
void CLogger::HandleLogPrint ( bool bTimeStamp, const char* szPrePend, const char* szMessage, bool bToConsole, bool bToLogFile )
{
    // Put the timestamp at the beginning of the string
#if 0
    string strOutputShort;
#endif
    string strOutputLong;
    if ( bTimeStamp )
    {
        char szBuffer [MAX_STRING_LENGTH] = { "\0" };
        time_t timeNow;
        time ( &timeNow );
        tm* pCurrentTime = localtime ( &timeNow );
#if 0
        if ( !strftime ( szBuffer, MAX_STRING_LENGTH - 1, "[%H:%M:%S] ", pCurrentTime ) )
            szBuffer[0] = 0;
        strOutputShort = szBuffer;
#endif
        if ( !strftime ( szBuffer, MAX_STRING_LENGTH - 1, "[%Y-%m-%d %H:%M:%S] ", pCurrentTime ) )
            szBuffer[0] = 0;
        strOutputLong = szBuffer;
    }

    // Build the final string
#if 0
    strOutputShort = strOutputShort + szPrePend + szMessage;
#endif
    strOutputLong = strOutputLong + szPrePend + szMessage;

    // Maybe print it in the console
#if 0   // Printing to the console is not currently used for this class
    if ( bToConsole )
        g_pServerInterface->Printf ( "%s", strOutputShort.c_str () );
#endif

    // Maybe print it to the log file
    if ( bToLogFile && m_pLogFile )
    {
        fprintf ( m_pLogFile, "%s", strOutputLong.c_str () );
        fflush ( m_pLogFile );
    }
}
