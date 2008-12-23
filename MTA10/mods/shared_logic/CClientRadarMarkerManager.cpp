/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarkerManager.cpp
*  PURPOSE:     Radar marker entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*
*****************************************************************************/

#include <StdInc.h>

CClientRadarMarkerManager::CClientRadarMarkerManager ( CClientManager* pManager )
{
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
    m_usDimension = 0;
    m_bOrderOnPulse = false;
}


CClientRadarMarkerManager::~CClientRadarMarkerManager ( void )
{
    // Remove all markers from the list
    DeleteAll ();
}


void CClientRadarMarkerManager::DoPulse ( void )
{   
    // Grab the camera position here (so it isn't done for every marker)
    m_pManager->GetCamera ()->GetPosition ( m_vecCameraPosition );
    list < CClientRadarMarker* > ::iterator iter = m_Markers.begin ();
    for ( ; iter != m_Markers.end (); iter++ )
    {
        (*iter)->DoPulse ();
    }
    if ( m_bOrderOnPulse )
    {
        OrderMarkers ();
        m_bOrderOnPulse = false;
    }
}


void CClientRadarMarkerManager::DeleteAll ( void )
{
    // Delete all the markers in our list
    m_bCanRemoveFromList = false;
    list < CClientRadarMarker* > ::const_iterator iter = m_Markers.begin ();
    for ( ; iter != m_Markers.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Markers.clear ();
    m_bCanRemoveFromList = true;
}


CClientRadarMarker* CClientRadarMarkerManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTRADARMARKER )
    {
        return static_cast < CClientRadarMarker* > ( pEntity );
    }

    return NULL;
}


void CClientRadarMarkerManager::RemoveFromList ( CClientRadarMarker* pMarker )
{
    if ( m_bCanRemoveFromList )
    {
        if ( !m_Markers.empty() ) m_Markers.remove ( pMarker );
    }
}


void CClientRadarMarkerManager::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    m_bOrderOnPulse = true;
}


bool CClientRadarMarkerManager::Exists ( CClientRadarMarker* pRadarMarker )
{
    list < CClientRadarMarker* > ::const_iterator iter = m_Markers.begin ();
    for ( ; iter != m_Markers.end () ; iter++ )
    {
        if ( *iter == pRadarMarker )
        {
            return true;
        }
    }

    return false;
}


void CClientRadarMarkerManager::OrderMarkers ( void )
{
    m_Markers.sort ( CompareOrderingIndex );

    list < CClientRadarMarker * > ::iterator iter = m_Markers.begin ();
    for ( ; iter != m_Markers.end () ; iter++ )
    {
        (*iter)->DestroyMarker ();
    }
    iter = m_Markers.begin ();
    for ( ; iter != m_Markers.end () ; iter++ )
    {
        if ( (*iter)->GetDimension() == m_usDimension )
        {
            (*iter)->CreateMarker ();
        }
    }
}


bool CClientRadarMarkerManager::CompareOrderingIndex ( CClientRadarMarker * p1, CClientRadarMarker * p2 )
{
    return p1->GetOrdering () < p2->GetOrdering ();
}