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
using std::list;

void* CClientStreamer::pAddingElement = NULL;

namespace
{

    float GetBoxDistanceSq ( const CVector& vecPosition, const CVector& vecBoxCenter, const float* fExtentMin, const float* fExtentMax, const CVector** vecBoxAxes )
    {
        CVector vecOffset = vecPosition - vecBoxCenter;
        float fDistSq = 0.f;

        // For each axis
        for ( int i = 0 ; i < 3 ; i++ )
        {
            // Project vecOffset on the axis
            float fDot = vecOffset.DotProduct ( vecBoxAxes[i] );

            // Add any distance outside the box on that axis
            if ( fDot < fExtentMin[i] )
                fDistSq += ( fDot - fExtentMin[i] ) * ( fDot - fExtentMin[i] );
            else
            if ( fDot > fExtentMax[i] )
                fDistSq += ( fDot - fExtentMax[i] ) * ( fDot - fExtentMax[i] );
        }

        return fDistSq;
    }

    // First draft. Works, but is not optimized.
    float GetElementStreamDistanceSquared ( CClientStreamElement* pElement, const CVector& vecPosition )
    {
        // Update cached radius if required
        if ( --pElement->m_iCachedRadiusCounter < 0 )
        {
            CStaticFunctionDefinitions::GetElementRadius ( *pElement, pElement->m_fCachedRadius );
            pElement->m_iCachedRadiusCounter = 20 + rand() % 50;
        }

        // Do a simple calculation if the element has a small radius (Using a big radius for now)
        if ( pElement->m_fCachedRadius < 20000 )
        {
            CVector vecDif = pElement->GetStreamPosition () - vecPosition;
            float fDist = sqrtf ( vecDif.fX * vecDif.fX + vecDif.fY * vecDif.fY + vecDif.fZ * vecDif.fZ );
            fDist = Max ( 0.0f, fDist - pElement->m_fCachedRadius );
            return fDist * fDist;
        }

        // Update cached bounding box if required
        if ( --pElement->m_iCachedBoundingBoxCounter < 0 )
        {
            // Get bounding box extents
            CVector vecMin;
            CVector vecMax;
            CStaticFunctionDefinitions::GetElementBoundingBox ( *pElement, vecMin, vecMax );

            // Adjust for non-centered bounding box
            CVector vecHalfCenter = ( vecMin + vecMax ) * 0.25f;
            vecMin -= vecHalfCenter;
            vecMax -= vecHalfCenter;

            pElement->m_vecCachedBoundingBox[0] = vecMin;
            pElement->m_vecCachedBoundingBox[1] = vecMax;

            pElement->m_iCachedBoundingBoxCounter = 20 + rand() % 50;
        }

        const CVector& vecMin = pElement->m_vecCachedBoundingBox[0];
        const CVector& vecMax = pElement->m_vecCachedBoundingBox[1];

        // Get bounding box axes
        CMatrix gtaMatrix;
        pElement->GetMatrix ( gtaMatrix );

        const CVector* vecBoxAxes[3] = { &gtaMatrix.vRight, &gtaMatrix.vFront, &gtaMatrix.vUp };

        return GetBoxDistanceSq ( vecPosition, pElement->GetStreamPosition (), &vecMin.fX, &vecMax.fX, vecBoxAxes );
   }


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
    for ( std::map < CClientStreamElement*, int >::iterator it = m_StreamedInMap.begin (); it != m_StreamedInMap.end (); )
    {
        if ( it->first->GetDimension () != m_usDimension )
        {
            m_StreamedInMap.erase ( it++ );
            DoStreamOut ( it->first );
        }
        else
            ++it;
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
                fDistSq = GetElementStreamDistanceSquared ( pElement, vecPosition );
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
