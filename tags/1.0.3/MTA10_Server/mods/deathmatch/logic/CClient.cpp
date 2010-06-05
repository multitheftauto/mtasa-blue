/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClient.cpp
*  PURPOSE:     Connected client class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

CClient::CClient ( void )
{
    m_pAccount = new CAccount ( g_pGame->GetAccountManager (), false, "guest" );
}


CClient::~CClient ( void )
{
    if ( m_pAccount )
    {
        m_pAccount->SetClient ( NULL );

        // If its a guest account, delete it
        if ( !m_pAccount->IsRegistered () )
            delete m_pAccount;
    }        
}


bool CClient::IsRegistered ( void )
{
    return m_pAccount->IsRegistered ();
}


CElement* CClient::GetElement ( void )
{
    CElement* pElement = NULL;
    switch ( GetClientType () )
    {
        case CLIENT_CONSOLE:
        {
            CConsoleClient* pClient = static_cast < CConsoleClient* > ( this );
            pElement = static_cast < CElement* > ( pClient );
            break;
        }
        case CLIENT_PLAYER:
        {
            CPlayer* pClient = static_cast < CPlayer* > ( this );
            pElement = static_cast < CElement* > ( pClient );
            break;
        }
    }
    return pElement;
}

