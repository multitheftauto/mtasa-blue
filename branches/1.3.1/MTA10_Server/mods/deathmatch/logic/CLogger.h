/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLogger.h
*  PURPOSE:     Server logger class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLOGGER_H
#define __CLOGGER_H

#include <cstdio>
#include "../Config.h"

enum eLogLevel
{
    LOGLEVEL_LOW       = 1,
    LOGLEVEL_MEDIUM    = 2,
};

class CLogger
{
public:
    static void         LogPrintf           ( const char* szFormat, ... );
    static void         LogPrint            ( const char* szText );

    static void         LogPrintf           ( eLogLevel logLevel, const char* szFormat, ... );
    static void         LogPrint            ( eLogLevel logLevel, const char* szText );

    static void         LogPrintfNoStamp    ( const char* szFormat, ... );
    static void         LogPrintNoStamp     ( const char* szText );

    static void         ErrorPrintf         ( const char* szFormat, ... );
    static void         DebugPrintf         ( const char* szFormat, ... );
    static void         AuthPrintf          ( const char* szFormat, ... );

    static bool         SetLogFile          ( const char* szLogFile );
    static bool         SetAuthFile         ( const char* szAuthFile );

    static void         SetMinLogLevel      ( eLogLevel logLevel );

    static void         ProgressDotsBegin   ( void );
    static void         ProgressDotsUpdate  ( void );
    static void         ProgressDotsEnd     ( void );

    static void         BeginConsoleOutputCapture  ( void );
    static SString      EndConsoleOutputCapture    ( void );

private:
    static void         HandleLogPrint     ( bool bTimeStamp, const char* szPrePend, const char* szMessage, bool bToConsole, bool bToLogFile, bool bToAuthFile, eLogLevel logLevel = LOGLEVEL_MEDIUM );

    static FILE*        m_pLogFile;
    static FILE*        m_pAuthFile;
    static eLogLevel    m_MinLogLevel;
    static bool         m_bPrintingDots;
    static SString      m_strCaptureBuffer;
    static bool         m_bCaptureConsole;
    static CCriticalSection m_CaptureBufferMutex;
};

#endif
