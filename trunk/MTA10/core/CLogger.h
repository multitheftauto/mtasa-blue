/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CLogger.h
*  PURPOSE:     Header file for logger class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLOGGER_H
#define __CLOGGER_H

#include "CSingleton.h"
#include <fstream>
#include <string>

#ifdef MTA_DEBUG
#   define WriteDebugEvent( a ) CLogger::GetSingleton ( ).WriteDebugEventInt ( a )
#   define WriteErrorEvent( a ) CLogger::GetSingleton ( ).WriteErrorEventInt ( a )
#else
#   define WriteDebugEvent( a ) CLogger::GetSingleton ( ).WriteDebugEventInt ( a )
#   define WriteErrorEvent( a ) CLogger::GetSingleton ( ).WriteErrorEventInt ( a )
#endif

class CLogger : public CSingleton < CLogger >
{
    public:
            CLogger ( );
           ~CLogger ( );

    void    WriteEvent          ( std::string EventDescription );
    void    WriteDebugEventInt  ( std::string EventDescription );
    void    OutputDebugMessage  ( std::string DebugMessage ); 
    void    WriteErrorEventInt  ( std::string ErrorMessage );
    void    ErrorPrintf         ( const char* szFormat, ... );

    private:
    
    std::fstream File;

};

#endif