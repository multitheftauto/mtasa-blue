/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CServer.cpp
 *  PURPOSE:     Server interface handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServer.h"
#include "CGame.h"
#include "CMainConfig.h"
#define ALLOC_STATS_MODULE_NAME "deathmatch"

#include "SharedUtil.hpp"
#include "SharedUtil.Thread.h"
#include "SharedUtil.IntervalCounter.h"
#include "SharedUtil.IntervalCounter.hpp"

#ifdef MTA_DEBUG
    #include "SharedUtil.Tests.hpp"
#endif

CServerInterface* g_pServerInterface = nullptr;
CNetServer*       g_pNetServer = nullptr;
CNetServer*       g_pRealNetServer = nullptr;

CServer::CServer() noexcept {}
CServer::~CServer() noexcept {}

void CServer::ServerInitialize(CServerInterface* pServerInterface) noexcept
{
    m_pServerInterface = pServerInterface;
    g_pServerInterface = pServerInterface;
    g_pNetServer = pServerInterface->GetNetwork();
    g_pRealNetServer = g_pNetServer;
    #ifdef MTA_DEBUG
    SharedUtil_Tests();
    #endif
}

bool CServer::ServerStartup(int iArgumentCount, char* szArguments[]) noexcept
{
    if (m_pGame)
        return false;

    m_pGame = new CGame;
    return m_pGame->Start(iArgumentCount, szArguments);
}

void CServer::ServerShutdown() noexcept
{
    if (!m_pGame)
        return;

    delete m_pGame;
    m_pGame = nullptr;
}

void CServer::GetTag(char* szInfoTag, int iInfoTag) const noexcept
{
    if (!m_pGame)
        return;

    m_pGame->GetTag(szInfoTag, iInfoTag);
}

void CServer::HandleInput(const char* szCommand) noexcept
{
    if (!m_pGame)
        return;

    m_pGame->HandleInput(szCommand);
}

void CServer::DoPulse() noexcept
{
    UNCLOCK(" Top", " Idle");
    if (m_pGame)
    {
        CLOCK(" Top", "Game->DoPulse");
        m_pGame->DoPulse();
        UNCLOCK(" Top", "Game->DoPulse");
    }
    CLOCK(" Top", " Idle");
}

bool CServer::IsFinished() const noexcept
{
    return m_pGame ? m_pGame->IsFinished() : false;
}

bool CServer::PendingWorkToDo() noexcept
{
    if (!m_pGame || !g_pNetServer)
        return false;

    return g_pNetServer->GetPendingPacketCount() > 0;
}

bool CServer::GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit) const noexcept
{
    if (!m_pGame || !g_pNetServer)
        return false;

    iSleepBusyMs = m_pGame->GetConfig()->GetPendingWorkToDoSleepTime();
    iSleepIdleMs = m_pGame->GetConfig()->GetNoWorkToDoSleepTime();
    iLogicFpsLimit = m_pGame->GetConfig()->GetServerLogicFpsLimit();
    return true;
}
