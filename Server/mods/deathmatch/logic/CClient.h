/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClient.h
 *  PURPOSE:     Connected client class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CAccount;
class CElement;

class CClient
{
public:
    enum
    {
        CLIENT_CONSOLE,
        CLIENT_PLAYER,
        CLIENT_REMOTECLIENT,
        CLIENT_SCRIPT,
    };

    enum eQuitReasons
    {
        QUIT_QUIT,
        QUIT_KICK,
        QUIT_BAN,
        QUIT_CONNECTION_DESYNC,
        QUIT_TIMEOUT,
    };

public:
    CClient(bool bAddGuestAccount = true);
    ~CClient();

    virtual int         GetClientType() = 0;
    virtual const char* GetNick() = 0;

    bool IsRegistered();

    virtual void SendEcho(const char* szEcho) = 0;
    virtual void SendConsole(const char* szEcho) = 0;

    CAccount* GetAccount() { return m_pAccount; }
    void      SetAccount(CAccount* pAccount) { m_pAccount = pAccount; }

    CElement* GetElement();

protected:
    CAccount* m_pAccount;
};
