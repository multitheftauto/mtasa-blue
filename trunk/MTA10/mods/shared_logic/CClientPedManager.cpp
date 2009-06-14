/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedManager.cpp
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientPedManager::CClientPedManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bRemoveFromList = true;
}


CClientPedManager::~CClientPedManager ( void )
{
    DeleteAll ();
}


void CClientPedManager::DeleteAll ( void )
{
    m_bRemoveFromList = false;
    list < CClientPed* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_List.clear ();
    m_bRemoveFromList = true;
}


void CClientPedManager::DoPulse ( void )
{   
    CClientPed * pPed = NULL;
    // Loop through our streamed-in peds
    list < CClientPed * > List = m_StreamedIn;
    list < CClientPed* > ::iterator iter = List.begin ();
    for ( ; iter != List.end (); iter++ )
    {
        pPed = *iter;
        // We should have a game ped here
        assert ( pPed->GetGamePlayer () );
        pPed->StreamedInPulse ();
    }
}


CClientPed* CClientPedManager::Get ( ElementID ID, bool bCheckPlayers )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && ( pEntity->GetType () == CCLIENTPED || ( bCheckPlayers && pEntity->GetType () == CCLIENTPLAYER ) ) )
    {
        return static_cast < CClientPed* > ( pEntity );
    }

    return NULL;
}


CClientPed* CClientPedManager::Get ( CPlayerPed* pPlayer, bool bValidatePointer, bool bCheckPlayers )
{
    if ( !pPlayer ) return NULL;

    if ( bValidatePointer )
    {
        list < CClientPed* > ::const_iterator iter = m_StreamedIn.begin ();
        for ( ; iter != m_StreamedIn.end (); iter++ )
        {
            if ( (*iter)->GetGamePlayer () == pPlayer )
            {
                return *iter;
            }
        }
    }
    else
    {
        CClientPed* pPed = reinterpret_cast < CClientPed* > ( pPlayer->GetStoredPointer () );
        if ( pPed->GetType () == CCLIENTPED || bCheckPlayers )
        {
            return pPed;
        }
    }
    return NULL;
}


CClientPed* CClientPedManager::GetSafe ( CEntity * pEntity, bool bCheckPlayers )
{
    if ( !pEntity ) return NULL;

    list < CClientPed* > ::const_iterator iter = m_StreamedIn.begin ();
    for ( ; iter != m_StreamedIn.end (); iter++ )
    {
        if ( dynamic_cast < CEntity * > ( (*iter)->GetGamePlayer () ) == pEntity )
        {
            return *iter;
        }
    }
    return NULL;
}


bool CClientPedManager::Exists ( CClientPed* pPed )
{
    // Is it in our list?
    list < CClientPed* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pPed )
            return true;
    }

    // Nope
    return false;
}


void CClientPedManager::RemoveFromList ( CClientPed* pPed )
{
    if ( m_bRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pPed );
    }
}


void CClientPedManager::OnCreation ( CClientPed * pPed )
{
    m_StreamedIn.push_back ( pPed );
}


void CClientPedManager::OnDestruction ( CClientPed * pPed )
{
    m_StreamedIn.remove ( pPed );
}
