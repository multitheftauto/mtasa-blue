/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CLogger.h
*  PURPOSE:     Logger class header
*  DEVELOPERS:  Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CLOGGER_H
#define __CLOGGER_H

#include <cstdio>
//#include "../Config.h"

class CLogger
{
public:
    static void         LogPrintf           ( const char* szFormat, ... );
    static void         LogPrint            ( const char* szText );

    static void         LogPrintfNoStamp    ( const char* szFormat, ... );
    static void         LogPrintNoStamp     ( const char* szText );

    static void         ErrorPrintf         ( const char* szFormat, ... );
    static void         DebugPrintf         ( const char* szFormat, ... );

    static void         SetLogFile          ( const char* szLogFile );

private:
    static void         HandleLogPrint     ( bool bTimeStamp, const char* szPrePend, const char* szMessage, bool bToConsole, bool bToLogFile );

    static FILE*        m_pLogFile;
};

#endif
