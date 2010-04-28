/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarAreaManager.cpp
*  PURPOSE:     Radar area entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Chris McArthur <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientRadarAreaManager::CClientRadarAreaManager ( CClientManager* pManager )
{
    // Init
    m_pManager = pManager;
    m_bDontRemoveFromList = false;
    m_usDimension = 0;
}


CClientRadarAreaManager::~CClientRadarAreaManager ( void )
{
    // Make sure all the radar areas are deleted
    DeleteAll ();
}


CClientRadarArea* CClientRadarAreaManager::Create ( ElementID EntityID )
{
    if ( !Get ( EntityID ) )
    {
        return new CClientRadarArea ( m_pManager, EntityID );
    }

    return NULL;
}


void CClientRadarAreaManager::Delete ( CClientRadarArea* pRadarArea )
{
    delete pRadarArea;
}


void CClientRadarAreaManager::DeleteAll ( void )
{
    // Delete all the radar areas
    m_bDontRemoveFromList = true;
    list < CClientRadarArea* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }
    m_bDontRemoveFromList = false;

    // Clear the list
    m_List.clear ();
}

CClientRadarArea* CClientRadarAreaManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTRADARAREA )
    {
        return static_cast < CClientRadarArea* > ( pEntity );
    }

    return NULL;
}


void CClientRadarAreaManager::SetDimension ( unsigned short usDimension )
{
    list < CClientRadarArea* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        (*iter)->RelateDimension ( usDimension );
    }

    m_usDimension = usDimension;
}


void CClientRadarAreaManager::DoPulse ( void )
{
    // Always assume that we are rendering
    DoPulse ( true );
}

// DoPulse with a rendering flag, as this can be called from either a hook (render == true) or the CClientManager::DoPulse (render == false)
void CClientRadarAreaManager::DoPulse ( bool bRender )
{
    // Pulse each radar area marker
    list < CClientRadarArea* > ::const_iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); ++iter )
    {
        (*iter)->DoPulse ( bRender );
    }
}

void CClientRadarAreaManager::RemoveFromList ( CClientRadarArea* pRadarArea )
{
    if ( !m_bDontRemoveFromList )
    {
        if ( !m_List.empty() ) m_List.remove ( pRadarArea );
    }
}
