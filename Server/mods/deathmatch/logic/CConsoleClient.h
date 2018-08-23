/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsoleClient.h
 *  PURPOSE:     Console connected client class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClient.h"
#include "CElement.h"
#include "CConsole.h"
#include "CLogger.h"

class CConsoleClient : public CElement, public CClient
{
public:
    CConsoleClient(CConsole* pConsole);
    virtual ~CConsoleClient(void){};

    int         GetClientType(void) { return CClient::CLIENT_CONSOLE; };
    const char* GetNick(void) { return m_strNick; };

    void SendEcho(const char* szEcho) { CLogger::LogPrintf("%s\n", szEcho); };
    void SendConsole(const char* szEcho) { CLogger::LogPrintf("%s\n", szEcho); };

    void Unlink(void){};
    bool ReadSpecialData(void) { return false; };

protected:
    SString   m_strNick;
    CConsole* m_pConsole;
};
