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
    CServer() noexcept;
    virtual ~CServer() noexcept;

    void ServerInitialize(CServerInterface* pServerInterface) noexcept;
    bool ServerStartup(int iArgumentCount, char* szArguments[]) noexcept;
    void ServerShutdown() noexcept;

    void DoPulse() noexcept;

    void GetTag(char* szInfoTag, int iInfoTag) const noexcept;
    void HandleInput(const char* szCommand) noexcept;

    bool IsFinished() const noexcept;
    bool PendingWorkToDo() noexcept;
    bool GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit) const noexcept;

private:
    CServerInterface* m_pServerInterface = nullptr;
    class CGame*      m_pGame = nullptr;
};
