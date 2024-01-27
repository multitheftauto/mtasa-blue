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
#if defined(MTA_DEBUG)
    #include "SharedUtil.Tests.hpp"
#endif

CServerInterface* g_pServerInterface = NULL;
CNetServer*       g_pNetServer = NULL;
CNetServer*       g_pRealNetServer = NULL;

CServer::CServer()
{
    // Init
    m_pServerInterface = NULL;
    m_pGame = NULL;
}

CServer::~CServer()
{
}

void CServer::ServerInitialize(CServerInterface* pServerInterface)
{
    m_pServerInterface = pServerInterface;
    g_pServerInterface = pServerInterface;
    g_pNetServer = pServerInterface->GetNetwork();
    g_pRealNetServer = g_pNetServer;
    #if defined(MTA_DEBUG)
    SharedUtil_Tests();
    #endif
}

bool CServer::ServerStartup(int iArgumentCount, char* szArguments[])
{
    if (!m_pGame)
    {
        m_pGame = new CGame;
        return m_pGame->Start(iArgumentCount, szArguments);
    }

    return false;
}

void CServer::ServerShutdown()
{
    if (m_pGame)
    {
        delete m_pGame;
        m_pGame = NULL;
    }
}

void CServer::GetTag(char* szInfoTag, int iInfoTag)
{
    if (m_pGame)
    {
        m_pGame->GetTag(szInfoTag, iInfoTag);
    }
}

void CServer::HandleInput(char* szCommand)
{
    if (m_pGame)
    {
        m_pGame->HandleInput(szCommand);
    }
}

void CServer::DoPulse()
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

bool CServer::IsReadyToAcceptConnections() const noexcept
{
    return (m_pGame != nullptr) && m_pGame->IsServerFullyUp();
}

bool CServer::IsFinished()
{
    if (m_pGame)
    {
        return m_pGame->IsFinished();
    }

    return false;
}

bool CServer::PendingWorkToDo()
{
    if (m_pGame && g_pNetServer)
    {
        if (g_pNetServer->GetPendingPacketCount() > 0)
        {
            return true;
        }
    }
    return false;
}

bool CServer::GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit)
{
    if (m_pGame && g_pNetServer)
    {
        iSleepBusyMs = m_pGame->GetConfig()->GetPendingWorkToDoSleepTime();
        iSleepIdleMs = m_pGame->GetConfig()->GetNoWorkToDoSleepTime();
        iLogicFpsLimit = m_pGame->GetConfig()->GetServerLogicFpsLimit();
        return true;
    }
    return false;
}
