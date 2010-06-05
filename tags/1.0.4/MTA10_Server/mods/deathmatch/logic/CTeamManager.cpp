/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTeamManager.cpp
*  PURPOSE:     Team element manager class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTeamManager::CTeamManager ( void )
{
    m_bRemoveFromList = true;
}


CTeamManager::~CTeamManager ( void )
{
    RemoveAll ();
}


void CTeamManager::RemoveFromList ( CTeam* pTeam )
{
    if ( m_bRemoveFromList && !m_List.empty() )
    {
        m_List.remove ( pTeam );
    }
}


void CTeamManager::RemoveAll ( void )
{
    m_bRemoveFromList = false;

    list < CTeam* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_List.clear ();

    m_bRemoveFromList = true;
}


CTeam* CTeamManager::GetTeam ( const char* szName )
{
    if ( szName == NULL ) return NULL;

    list < CTeam* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( strcmp ( (*iter)->GetTeamName (), szName ) == 0 )
            return *iter;
    }
    
    return NULL;
}


bool CTeamManager::Exists ( CTeam* pTeam )
{
    list < CTeam* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pTeam )
            return true;
    }

    return false;
}


CTeam* CTeamManager::Create ( CElement* pParent, CXMLNode* pNode, char* szName, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    // Create the team
    CTeam* pTeam = new CTeam ( this, pParent, pNode, szName, ucRed, ucGreen, ucBlue );

    // Invalid Team id?
    if ( pTeam->GetID () == INVALID_ELEMENT_ID )
    {
        delete pTeam;
        return NULL;
    }

    // Return the created Team
    return pTeam;
}


CTeam* CTeamManager::CreateFromXML ( CElement* pParent, CXMLNode& Node, CLuaMain* pLuaMain, CEvents* pEvents )
{
    // Create the Team
    CTeam* pTeam = new CTeam ( this, pParent, &Node );

    // Verify the Team id and load the data from xml
    if ( pTeam->GetID () == INVALID_ELEMENT_ID ||
         !pTeam->LoadFromCustomData ( pLuaMain, pEvents ) )
    {
        delete pTeam;
        return NULL;
    }

    // Return the created Team
    return pTeam;
}
