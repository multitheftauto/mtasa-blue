/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientFireManager.cpp
*  PURPOSE:     Fire entity manager class
*  DEVELOPERS:  Fedor Sinev
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientFireManager::CClientFireManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
}


CClientFireManager::~CClientFireManager ( void )
{
    DeleteAll ( );
}

CClientFire* CClientFireManager::Get ( ElementID ID )
{
    // Retrive element id
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTWATER )
    {
        return static_cast < CClientFire* > ( pEntity );
    }

    return NULL;
}


void CClientFireManager::DeleteAll ( void )
{
    // Delete through the list
    m_bDontRemoveFromList = true;
    list < CClientFire* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
}


bool CClientFireManager::Exists ( CClientFire* pFire )
{
    list < CClientFire* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        if ( *iter == pFire )
        {
            return true;
        }
    }
    return false;
}