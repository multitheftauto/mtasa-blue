/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWaterManager.cpp
*  PURPOSE:     Water entity manager class
*  DEVELOPERS:  arc_
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientWaterManager::CClientWaterManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
}


CClientWaterManager::~CClientWaterManager ( void )
{
    DeleteAll ();
}


CClientWater* CClientWaterManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTWATER )
    {
        return static_cast < CClientWater* > ( pEntity );
    }

    return NULL;
}


void CClientWaterManager::DeleteAll ( void )
{
    // Delete each water poly
    m_bDontRemoveFromList = true;
    list < CClientWater* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
}


bool CClientWaterManager::Exists ( CClientWater* pWater )
{
    list < CClientWater* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( *iter == pWater )
        {
            return true;
        }
    }
    return false;
}

void CClientWaterManager::AddToList ( CClientWater* pWater )
{
    m_List.push_back ( pWater );
}

void CClientWaterManager::RemoveFromList ( CClientWater* pWater )
{
    if ( !m_bDontRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pWater );
    }
}
