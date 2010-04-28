/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/sockets/CTCPServerSocketImplManager.cpp
*  PURPOSE:     TCP server socket manager class
*  DEVELOPERS:  Oli <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CTCPServerSocketImpl.h"
#include "CTCPServerSocketImplManager.h"

using std::list;

CTCPServerSocketImplManager::CTCPServerSocketImplManager ( void )
{
}


CTCPServerSocketImplManager::~CTCPServerSocketImplManager ( void )
{
    list < CTCPServerSocketImpl* > ::iterator iter = m_Servers.begin ();
    for ( ; iter != m_Servers.end (); iter++ )
    {
        delete *iter;
    }

    m_Servers.clear ();
}


void CTCPServerSocketImplManager::AddServer ( CTCPServerSocketImpl* pServer )
{
    m_Servers.push_back ( pServer );
}


void CTCPServerSocketImplManager::RemoveServer ( CTCPServerSocketImpl* pServer )
{
    if ( !m_Servers.empty() ) m_Servers.remove ( pServer );
    delete pServer;
}