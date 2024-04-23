/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CServer.h
 *  PURPOSE:     Server interface handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CServerBase.h>

class CServer : public CServerBase
{
public:
    CServer();
    virtual ~CServer();

    void ServerInitialize(CServerInterface* pServerInterface);
    bool ServerStartup(int iArgumentCount, char* szArguments[]);
    void ServerShutdown();

    void DoPulse();

    void GetTag(char* szInfoTag, int iInfoTag);
    void HandleInput(char* szCommand);

    bool IsReadyToAcceptConnections() const noexcept override;
    bool IsFinished();
    bool PendingWorkToDo();
    bool GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit);

private:
    CServerInterface* m_pServerInterface;
    class CGame*      m_pGame;
};
