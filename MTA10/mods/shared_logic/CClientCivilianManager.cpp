/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCivilianManager.cpp
*  PURPOSE:     Civilian ped manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientCivilianManager::CClientCivilianManager ( CClientManager* pManager )
{
    // Initialize members
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
}


CClientCivilianManager::~CClientCivilianManager ( void )
{
    // Destroy all vehicles
    DeleteAll ();
}


CClientCivilian* CClientCivilianManager::Create ( int iPedModel, ElementID ID )
{
    return new CClientCivilian ( m_pManager, ID, iPedModel );
}


CClientCivilian* CClientCivilianManager::Create ( CCivilianPed * pPed, ElementID ID )
{
    return new CClientCivilian ( m_pManager, ID, pPed );
}


void CClientCivilianManager::DeleteAll ( void )
{
    // Delete all the civilians
    m_bCanRemoveFromList = false;
    list < CClientCivilian* > ::const_iterator iter = m_Civilians.begin ();
    for ( ; iter != m_Civilians.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Civilians.clear ();
    m_bCanRemoveFromList = true;
}


CClientCivilian* CClientCivilianManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTCIVILIAN )
    {
        return static_cast < CClientCivilian* > ( pEntity );
    }

    return NULL;
}


bool CClientCivilianManager::IsValidModel ( unsigned int ulVehicleModel )
{
    return false;
}


list < CClientCivilian* > ::iterator CClientCivilianManager::IterGet ( CClientCivilian* pVehicle )
{
    // Find it in our list
    list < CClientCivilian* > ::iterator iter = m_Civilians.begin ();
    for ( ; iter != m_Civilians.end (); iter++ )
    {
        if ( *iter == pVehicle )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_Civilians.begin ();
}


list < CClientCivilian* > ::reverse_iterator CClientCivilianManager::IterGetReverse ( CClientCivilian* pVehicle )
{
    // Find it in our list
    list < CClientCivilian* > ::reverse_iterator iter = m_Civilians.rbegin ();
    for ( ; iter != m_Civilians.rend (); iter++ )
    {
        if ( *iter == pVehicle )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_Civilians.rbegin ();
}


void CClientCivilianManager::RemoveFromList ( CClientCivilian* pCivilian )
{
    if ( m_bCanRemoveFromList )
    {
        if ( !m_Civilians.empty() ) m_Civilians.remove ( pCivilian );
    }
}
