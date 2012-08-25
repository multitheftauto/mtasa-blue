/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/CServer.cpp
*  PURPOSE:     Server interface handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define ALLOC_STATS_MODULE_NAME "deathmatch"
#include "SharedUtil.hpp"

CServerInterface* g_pServerInterface = NULL;
CNetServer* g_pNetServer = NULL;
CNetServer* g_pRealNetServer = NULL;

CServer::CServer ( void )
{
    // Init
    m_pServerInterface = NULL;
    m_pGame = NULL;
}


CServer::~CServer ( void )
{
    
}


void CServer::ServerInitialize ( CServerInterface* pServerInterface )
{
    m_pServerInterface = pServerInterface;
    g_pServerInterface = pServerInterface;
    g_pNetServer = pServerInterface->GetNetwork ();
    g_pRealNetServer = g_pNetServer;
}


bool CServer::ServerStartup ( int iArgumentCount, char* szArguments [] )
{
    if ( !m_pGame )
    {
        m_pGame = new CGame;
        return m_pGame->Start ( iArgumentCount, szArguments );
    }

    return false;
}


void CServer::ServerShutdown ( void )
{
    if ( m_pGame )
    {
        delete m_pGame;
        m_pGame = NULL;
    }
}

void CServer::GetTag ( char* szInfoTag, int iInfoTag )
{
    if ( m_pGame )
    {
        m_pGame->GetTag ( szInfoTag, iInfoTag );
    }
}

void CServer::HandleInput ( char* szCommand )
{
    if ( m_pGame )
    {
        m_pGame->HandleInput ( szCommand );
    }
}


void CServer::DoPulse()
{
    UNCLOCK( " Top", " Idle" );
    if ( m_pGame )
    {
        CLOCK( " Top", "Game->DoPulse" );
        m_pGame->DoPulse ();
        UNCLOCK( " Top", "Game->DoPulse" );
    }
    CLOCK( " Top", " Idle" );
}


bool CServer::IsFinished ()
{
    if ( m_pGame )
    {
        return m_pGame->IsFinished ();
    }

    return false;
}

bool CServer::PendingWorkToDo ( int& iSleepMs )
{
    if ( m_pGame && g_pNetServer )
    {
        if ( g_pNetServer->GetPendingPacketCount () > 0 )
        {
            iSleepMs = m_pGame->GetConfig ()->GetPendingWorkToDoSleepTime ();
            return true;
        }
        else
        {
            iSleepMs = m_pGame->GetConfig ()->GetNoWorkToDoSleepTime ();
            return true;
        }
    }
    return false;
}
