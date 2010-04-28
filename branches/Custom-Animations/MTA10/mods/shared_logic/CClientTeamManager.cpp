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

using std::list;

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

    list < CClientTeam* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_List.clear ();

    m_bRemoveFromList = true;
}


bool CClientTeamManager::Exists ( CClientTeam* pTeam )
{
    list < CClientTeam* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pTeam )
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
    list < CClientTeam* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( strcmp ( (*iter)->GetTeamName (), szName ) == 0 )
            return *iter;
    }
    
    return NULL;
}
