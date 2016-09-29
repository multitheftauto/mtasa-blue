/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTeamManager.cpp
*  PURPOSE:     Team entity manager class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*
*****************************************************************************/

#include <StdInc.h>

CClientTeamManager::CClientTeamManager ( void )
{
    m_bRemoveFromList = true;
}


CClientTeamManager::~CClientTeamManager ( void )
{
    RemoveAll ();
}


void CClientTeamManager::RemoveFromList ( CClientTeam* pTeam )
{
    if ( m_bRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pTeam );
    }
}

void CClientTeamManager::RemoveAll ( void )
{
    m_bRemoveFromList = false;

    for (auto& pTeam : m_List)
    {
        delete pTeam;
    }

    m_List.clear ();

    m_bRemoveFromList = true;
}


bool CClientTeamManager::Exists ( CClientTeam* pTeam )
{
    for (auto& pIterTeam : m_List)
    {
        if ( pIterTeam == pTeam )
        {
            return true;
        }
    }
    return false;
}


CClientTeam* CClientTeamManager::GetTeam ( ElementID ID )
{
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTTEAM )
    {
        return static_cast < CClientTeam* > ( pEntity );
    }

    return NULL;
}


CClientTeam* CClientTeamManager::GetTeam ( const char* szName )
{
    for (auto& pTeam : m_List)
    {
        if ( strcmp ( pTeam->GetTeamName (), szName ) == 0 )
            return pTeam;
    }
    
    return NULL;
}
