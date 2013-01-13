/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectRespawner.cpp
*  PURPOSE:     Manager for objects to be respawned
*  DEVELOPERS:  Jusonex <jusonex96@gmail.com>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CObjectRespawner.h"

CObjectRespawner::CObjectRespawner ()
{
    // Allow unrefernces
    m_bAllowUnreference = true;
}


void CObjectRespawner::Respawn ( class CClientObject* pObject )
{
    // Make sure we don't try to delete it twice
    if ( pObject && !IsBeingRespawned ( pObject ) )
    {
        // Add it to our list
        if ( !pObject->IsBeingRespawned () )
        {
            m_List.push_back ( pObject );
        }

        // Flag it as being respawned
        pObject->SetBeingRespawned ( true );
    }
}


void CObjectRespawner::DoRespawnAll ( void )
{
    // Make sure elements won't call us back and screw with our list (would crash)
    m_bAllowUnreference = false;

    // Delete all the elements
    list < CClientObject* > ::iterator iter = m_List.begin ();
    while ( iter != m_List.end () )
    {
        CClientObject* pObject = *iter;

        pObject->ReCreate ();
        iter = m_List.erase ( iter );
        pObject->SetBeingRespawned ( false );
    }

    // We can now allow unrefernecs again
    m_bAllowUnreference = true;
}


bool CObjectRespawner::IsBeingRespawned ( CClientObject* pObject )
{
    return m_List.Contains ( pObject );
}


void CObjectRespawner::Unreference ( class CClientObject* pObject )
{
    // If we allow unreferencing, remove this element from the to delete list.
    if ( m_bAllowUnreference )
    {
        m_List.remove ( pObject );
    }
}
