/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConsoleClient.h
*  PURPOSE:     Console connected client class
*  DEVELOPERS:  Oliver Brown <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLECLIENT_H
#define __CCONSOLECLIENT_H

#include "CClient.h"
#include "CElement.h"
#include "CConsole.h"
#include "CLogger.h"

class CConsoleClient : public CElement, public CClient
{
public:
                            CConsoleClient          ( CConsole* pConsole );
    virtual                 ~CConsoleClient         ( void )                { };

    inline int              GetClientType           ( void )                { return CClient::CLIENT_CONSOLE; };
    inline const char*      GetNick                 ( void )                { return m_strNick; };

    inline void             SendEcho                ( const char* szEcho )  { CLogger::LogPrintf ( "%s\n", szEcho ); };
    inline void             SendConsole             ( const char* szEcho )  { CLogger::LogPrintf ( "%s\n", szEcho ); };

    inline void             Unlink                  ( void )                { };
    inline bool             ReadSpecialData         ( void )                { return false; };


protected:
    SString                 m_strNick;
    CConsole*               m_pConsole;

};

#endif
