/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CConsoleClient.h
 *  PURPOSE:     Console connected client class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CClient.h"
#include "CElement.h"
#include "CConsole.h"
#include "CLogger.h"

class CConsoleClient final : public CElement, public CClient
{
public:
    CConsoleClient(CConsole* pConsole);
    virtual ~CConsoleClient(){};

    int         GetClientType() { return CClient::CLIENT_CONSOLE; };
    const char* GetNick() { return m_strNick; };

    void SendEcho(const char* szEcho) { CLogger::LogPrintf("%s\n", szEcho); };
    void SendConsole(const char* szEcho) { CLogger::LogPrintf("%s\n", szEcho); };

    void Unlink(){};

protected:
    bool ReadSpecialData(const int iLine) override { return false; }

protected:
    SString   m_strNick;
    CConsole* m_pConsole;
};
