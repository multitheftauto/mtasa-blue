/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamer.cpp
*  PURPOSE:     Streamer class
*  DEVELOPERS:  Batman
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    // Used for sorting elements by distance
    struct SElementSortItem
    {
        SElementSortItem ( void ) {}
        SElementSortItem ( CClientStreamElement* pElement, float fDist ) : pElement ( pElement ) , fDist ( fDist ) {}

        bool operator < ( const SElementSortItem& other ) const
        {
            return fDist < other.fDist;
        }

        CClientStreamElement*   pElement;
        float                   fDist;
    };
}


CClientStreamer::CClientStreamer ( StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance, eClientEntityType entityType, unsigned int uiMaxCount )
{
    m_pLimitReachedFunc = pLimitReachedFunc;
    m_fMaxDistance = fMaxDistance;
    m_EntityType = entityType;
    m_uiMaxCount = uiMaxCount - 1;
}


CClientStreamer::~CClientStreamer ( void )
{
}


void CClientStreamer::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
}


void CClientStreamer::OnUpdateStreamPosition ( CClientStreamElement * pElement )
{
}


void CClientStreamer::AddElement ( CClientStreamElement * pElement )
{
    elementList.push_back ( pElement );
}


void CClientStreamer::RemoveElement ( CClientStreamElement * pElement )
{
    ListRemove ( elementList, pElement );
    MapRemove ( m_StreamedInMap, pElement );
    MapRemove ( m_ForcedInMap, pElement );
}


void CClientStreamer::OnElementForceStreamIn ( CClientStreamElement * pElement )
{
    // Make sure we're streamed in
    pElement->InternalStreamIn ( true );
    MapSet ( m_ForcedInMap, pElement, 1 );
}


void CClientStreamer::OnElementUnforceStreamIn ( CClientStreamElement * pElement )
{
    // Allow to be streamed out
    MapRemove ( m_ForcedInMap, pElement );
}


void CClientStreamer::OnElementDimension ( CClientStreamElement * pElement )
{
}


void CClientStreamer::NotifyElementStreamedIn ( CClientStreamElement * pElement )
{
    MapSet ( m_StreamedInMap, pElement, 1 );
}


void CClientStreamer::NotifyElementStreamedOut ( CClientStreamElement * pElement )
{
    MapRemove ( m_StreamedInMap, pElement );
}


void CClientStreamer::DoPulse ( const CVector& vecPosition )
{
#ifdef MTA_DEBUG
    for ( std::vector < CClientStreamElement* > ::iterator it = elementList.begin (); it != elementList.end (); it++ )
    {
        CClientStreamElement* pElement = *it;
        if ( pElement->GetBaseType () != m_EntityType )
            CLogger::ErrorPrintf ( "ClientStreamer::DoPulse type is %d(%d) but wanted %d", pElement->GetType (), pElement->GetBaseType (), m_EntityType );
    }
#endif

    //
    // Validate m_StreamedInMap items are still relevant
    //
    {
        std::vector < CClientStreamElement* > wrongList;

        // Make list of streamed in elements that are now in the wrong dimension
        for ( std::map < CClientStreamElement*, int >::iterator it = m_StreamedInMap.begin (); it != m_StreamedInMap.end (); ++it )
        {
            if ( it->first->GetDimension () != m_usDimension )
                wrongList.push_back ( it->first );
        }

        // And then stream them out
        for ( std::vector < CClientStreamElement* >::iterator it = wrongList.begin (); it != wrongList.end (); ++it )
            DoStreamOut ( *it );
    }

    //
    // Create a map of relevant elements
    //
    std::map < CClientStreamElement*, int > considerMap;
    {
        // Find entites within range
        CClientEntityResult result;
        GetClientSpatialDatabase ()->SphereQuery ( result, CSphere ( vecPosition, m_fMaxDistance ) );

        // Filter for required type and dimension
        for ( CClientEntityResult::iterator it = result.begin (); it != result.end (); it++ )
        {
            CClientEntity* pEntity = *it;
            if ( pEntity->GetBaseType () == m_EntityType && pEntity->GetDimension () == m_usDimension )
                considerMap[ (CClientStreamElement*)pEntity ] = 1;
        }

        // Merge elements we have streamed in already
        for ( std::map < CClientStreamElement*, int >::iterator it = m_StreamedInMap.begin (); it != m_StreamedInMap.end (); it++ )
        {
            considerMap[ it->first ] = 1;
        }

        // Merge forced items
        for ( std::map < CClientStreamElement*, int > ::iterator it = m_ForcedInMap.begin (); it != m_ForcedInMap.end (); it++ )
        {
            considerMap[ it->first ] = 1;
        }
    }

    //
    // Create a distance sorted list of relevant elements from the map
    //
    std::vector < SElementSortItem > sortList;
    {
        // Add consider items
        for ( std::map < CClientStreamElement*, int > ::iterator it = considerMap.begin (); it != considerMap.end (); it++ )
        {
            CClientStreamElement* pElement = it->first;
            float fDistSq;
            if ( MapContains ( m_ForcedInMap, pElement ) )
                fDistSq = 1;        // Make forced items really close so sorting will give them top priority
            else
                fDistSq = pElement->GetDistanceToBoundingBoxSquared ( vecPosition );
            sortList.push_back ( SElementSortItem ( pElement, fDistSq ) );
        }

        // Sort by distance
        std::sort ( sortList.begin (), sortList.end () );
    }


    //
    // Now we have a list of relevant items, sorted by distance - Some are streamed in, and some are not.
    //

    // 1 - Stream out everything beyond max distance
    // 2 - Starting with nearest, stream in everything until limit reached
    // 3 - Count what is left
    // 4 - Predict new limit by adding on those that are streamed in
    // 5 - Stream out everything above new limit
    // 6 - Try to stream in to new limit

    //
    // 1 - Stream out everything beyond max distance, or if it would exceed the max count
    //
    const float fMaxDistanceSq = m_fMaxDistance * m_fMaxDistance;
    int iLastIndex;
    for ( iLastIndex = sortList.size () - 1 ; iLastIndex != -1 ; iLastIndex-- )
    {
        const SElementSortItem& item = sortList[ iLastIndex ];
        if ( iLastIndex <= (int)m_uiMaxCount && item.fDist <= fMaxDistanceSq )
            break;
        DoStreamOut ( item.pElement );
    }

    //
    // 2 - Starting with nearest, stream in everything until limit reached
    //
    int iIndex;
    for ( iIndex = 0 ; iIndex <= iLastIndex ; iIndex++ )
    {
        if ( ReachedLimit () )
            break;

        const SElementSortItem& item = sortList[ iIndex ];
        DoStreamIn ( item.pElement );
    }

    //
    // 3 - Count what is left
    //
    const int iNumLeft = iLastIndex - iIndex;
    int iNumLeftIn = 0;
    int iNumLeftOut = 0;
    for ( int i = iIndex ; i <= iLastIndex ; i++ )
    {
        const SElementSortItem& item = sortList[ i ];
        if ( IsStreamedIn ( item.pElement ) )
            iNumLeftIn++;
    }
    iNumLeftOut = iNumLeft - iNumLeftIn;

    //
    // 4 - Predict new limit by adding on those that are streamed in
    //
    const int iNewLastIndex = iLastIndex + iNumLeftIn;

    //
    // 5 - Stream out everything above new limit
    //
    for ( int i = sortList.size () - 1 ; i > iNewLastIndex ; i-- )
    {
        const SElementSortItem& item = sortList[ i ];
        DoStreamOut ( item.pElement );
    }

    //
    // 6 - Try to stream in to new limit
    //
    for ( ; iIndex <= iNewLastIndex ; iIndex++ )
    {
        if ( ReachedLimit () )
            break;

        const SElementSortItem& item = sortList[ iIndex ];
        DoStreamIn ( item.pElement );
    }
}


void CClientStreamer::DoStreamOut ( CClientStreamElement * pElement )
{
    if ( !pElement->IsStreamedIn () )
        return;
    pElement->InternalStreamOut ();
}


void CClientStreamer::DoStreamIn ( CClientStreamElement * pElement )
{
    if ( pElement->IsStreamedIn () )
        return;
    bool bASync = g_pCore->GetGame ()->IsASyncLoadingEnabled ();
    pElement->InternalStreamIn ( !bASync || false );
}


bool CClientStreamer::IsStreamedIn ( CClientStreamElement * pElement )
{
    return pElement->IsStreamedIn ();
}


bool CClientStreamer::ReachedLimit ( void )
{
    return m_pLimitReachedFunc ();
}
