/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamer.cpp
*  PURPOSE:     Streamer class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

// Return the distance between two points ^ 2
/*
inline float ExpDistanceBetweenPoints ( const CVector& vec1, const CVector& vec2 )
{
	float fDistanceX = vec2.fX - vec1.fX;
	float fDistanceY = vec2.fY - vec1.fY;
	float fDistanceZ = vec2.fZ - vec1.fZ;
    return ( fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ );
}
*/
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

    void* pAddingElement = NULL;

    // First draft. Works, but is not optimized.
    float GetElementStreamDistanceSquared ( CClientStreamElement* pElement, const CVector& vecPosition )
    {
        // Do a simple calculation if the element is newly added ( hack/fix for CClientSteamer::AddElement being called in the CClientStreamElement constructor )
        if ( pElement == pAddingElement )
        {
            CVector vecDif = pElement->GetStreamPosition () - vecPosition;
            return ( vecDif.fX * vecDif.fX + vecDif.fY * vecDif.fY + vecDif.fZ * vecDif.fZ );
        }

        // Do a simple calculation if the element has a small radius
        float fRadius  = 0;
        CStaticFunctionDefinitions::GetElementRadius ( *pElement, fRadius );
        if ( fRadius < 20 )
        {
            CVector vecDif = pElement->GetStreamPosition () - vecPosition;
            return ( vecDif.fX * vecDif.fX + vecDif.fY * vecDif.fY + vecDif.fZ * vecDif.fZ );
        }

        // Get bounding box extents
        CVector vecMin;
        CVector vecMax;
        CStaticFunctionDefinitions::GetElementBoundingBox ( *pElement, vecMin, vecMax );

        // Adjust for non-centered bounding box
        CVector vecHalfCenter = ( vecMin + vecMax ) * 0.25f;
        vecMin -= vecHalfCenter;
        vecMax -= vecHalfCenter;

        // Get bounding box axes
        CMatrix gtaMatrix;
        pElement->GetMatrix ( gtaMatrix );

        const CVector* vecBoxAxes[3] = { &gtaMatrix.vRight, &gtaMatrix.vFront, &gtaMatrix.vUp };

        return GetBoxDistanceSq ( vecPosition, pElement->GetStreamPosition (), &vecMin.fX, &vecMax.fX, vecBoxAxes );
   }

}

// Define our static variables
vector < CClientStreamer * > CClientStreamer::m_Streamers;
vector < CClientStreamSectorRow * > CClientStreamer::m_WorldRows;
vector < CClientStreamSectorRow * > CClientStreamer::m_ExtraRows;
CClientStreamSectorRow * CClientStreamer::m_pRow = NULL;
CClientStreamSector * CClientStreamer::m_pSector = NULL;
CVector CClientStreamer::m_vecSectorPosition;


CClientStreamer::CClientStreamer ( StreamerLimitReachedFunction* pLimitReachedFunc, float fMaxDistance )
{    
    // Store all streamers
    m_Streamers.push_back ( this );

    // Setup our distance variables
    m_fMaxDistanceExp = fMaxDistance * fMaxDistance;
    m_fMaxDistanceThreshold = ( fMaxDistance + 50.0f ) * ( fMaxDistance + 50.0f );
    m_usDimension = 0;

    // We need the limit reached func
    assert ( m_pLimitReachedFunc = pLimitReachedFunc );

    SetupSectors ();
}


CClientStreamer::~CClientStreamer ( void )
{
    // If this is our last streamer, remove our static sectors
    if ( m_Streamers.size () == 1 )
    {
        // Clear our mainland rows
        vector < CClientStreamSectorRow * > ::iterator iter = m_WorldRows.begin ();
        for ( ; iter != m_WorldRows.end () ; iter++ )
        {
            delete *iter;
        }
        m_WorldRows.clear ();
        
        // Clear our extra rows
        iter = m_ExtraRows.begin ();
        for ( ; iter != m_ExtraRows.end () ; iter++ )
        {
            delete *iter;
        }
        m_ExtraRows.clear ();
    }
    // Unlink our streamer
    ListRemove ( m_Streamers, this );
}


void CClientStreamer::SetupSectors ()
{
    // Setup our sectors if this is our first instance
    if ( m_Streamers.size () == 1 )
    {
        // Create our main world sectors covering the mainland
        CreateSectors ( &m_WorldRows,
                        CVector2D ( SECTOR_SIZE, ROW_SIZE ),
                        CVector2D ( -WORLD_SIZE, -WORLD_SIZE ),
                        CVector2D ( WORLD_SIZE, WORLD_SIZE ) );

        // Find our row and sector    
        m_pRow = FindOrCreateRow ( m_vecSectorPosition );
        m_pSector = m_pRow->FindOrCreateSector ( m_vecSectorPosition );
    }

    // Activate the current sector for this streamer
    vector < CClientStreamSector * > activateSectors, deactivateSectors;
    CClientStreamSector * pActiveSectors [ 9 ];
    m_pSector->GetSurroundingSectors ( pActiveSectors );
    pActiveSectors [ 8 ] = m_pSector;
    for ( int i = 0 ; i < 9 ; i++ ) activateSectors.push_back ( pActiveSectors [ i ] );

    // Start this streamer at our current sector
    OnSectorChange ( activateSectors, deactivateSectors );
}


void CClientStreamer::CreateSectors ( vector < CClientStreamSectorRow * > * pList, CVector2D & vecSize, CVector2D & vecBottomLeft, CVector2D & vecTopRight )
{
    // Creates our sectors within rows, filling up our rectangle, connecting each sector and row
    CClientStreamSector * pCurrent = NULL, * pPrevious = NULL, *pPreviousRowSector = NULL;
    CClientStreamSectorRow * pCurrentRow = NULL, * pPreviousRow = NULL;
    float fX = vecBottomLeft.fX, fY = vecBottomLeft.fY;
    while ( fY < vecTopRight.fY )
    {
        pCurrentRow = new CClientStreamSectorRow ( fY, fY + vecSize.fY );
        pList->push_back ( pCurrentRow );
        pCurrentRow->m_pBottom = pPreviousRow;
        if ( pPreviousRow ) pPreviousRow->m_pTop = pCurrentRow;
        while ( fX < vecTopRight.fX )
        {            
            pPrevious = pCurrent;
            pCurrent = new CClientStreamSector ( pCurrentRow, CVector2D ( fX, fY ), CVector2D ( fX + vecSize.fX, fY + vecSize.fY ) );
            pCurrentRow->Add ( pCurrent );
            pCurrent->m_pLeft = pPrevious;
            if ( pPrevious ) pPrevious->m_pRight = pCurrent;
            if ( pPreviousRowSector )
            {
                pCurrent->m_pBottom = pPreviousRowSector;
                pPreviousRowSector->m_pTop = pCurrent;
                pPreviousRowSector = pPreviousRowSector->m_pRight;
            }        
            fX += vecSize.fX;
        }
        pPrevious = NULL;
        pCurrent = NULL;
        pPreviousRow = pCurrentRow;
        pPreviousRowSector = pPreviousRow->Front ();
        fX = vecBottomLeft.fX;
        fY += vecSize.fY;
    }
}


void CClientStreamer::ConnectRow ( CClientStreamSectorRow * pRow )
{
    float fTop, fBottom;
    pRow->GetPosition ( fTop, fBottom );

    // Connect up our row
    pRow->m_pTop = FindRow ( fTop + ( ROW_SIZE / 2.0f ) );
    pRow->m_pBottom = FindRow ( fBottom - ( ROW_SIZE / 2.0f ) );

    // Connect the other rows to us
    if ( pRow->m_pTop ) pRow->m_pTop->m_pBottom = pRow;
    if ( pRow->m_pBottom ) pRow->m_pBottom->m_pTop = pRow;
}

    
#include "..\deathmatch\logic\CClientGame.h"
extern CClientGame* g_pClientGame;
void CClientStreamer::UpdateSectors ( CVector & vecPosition )
{
    /*
    // Debugging code
    CClientStreamSector * pSector;
    vector < CClientStreamSector * > ::iterator iterSector;
    vector < CClientStreamSectorRow * > ::iterator iterRow = m_WorldRows.begin ();
    for ( ; iterRow != m_WorldRows.end () ; iterRow++ )
    {
        iterSector = (*iterRow)->Begin ();
        for ( ; iterSector != (*iterRow)->End () ; iterSector++ )
        {
            pSector = *iterSector;
            if ( !pSector->m_pArea )
            {
                CVector2D vecSize = pSector->m_vecTopRight - pSector->m_vecBottomLeft;
                CVector2D vecPosition = pSector->m_vecBottomLeft + ( vecSize / CVector2D ( 2.0f, 2.0f ) );
                
                pSector->m_pArea = new CClientRadarArea ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID );
                pSector->m_pArea->SetPosition ( vecPosition );
                pSector->m_pArea->SetSize ( CVector2D ( 100, 100 ) );
                pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
            }
            if ( pSector == m_pSector ) pSector->m_pArea->SetColor ( 0, 255, 0, 255 );
            else if ( pSector->IsActivated () ) pSector->m_pArea->SetColor ( 255, 255, 0, 255 );
            else pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
        }
    }
    iterRow = m_ExtraRows.begin ();    
    for ( ; iterRow != m_ExtraRows.end () ; iterRow++ )
    {
        iterSector = (*iterRow)->Begin ();
        for ( ; iterSector != (*iterRow)->End () ; iterSector++ )
        {
            pSector = *iterSector;
            if ( !pSector->m_pArea )
            {
                pSector->m_pArea = new CClientRadarArea ( g_pClientGame->GetManager (), INVALID_ELEMENT_ID );
                pSector->m_pArea->SetPosition ( pSector->m_vecBottomLeft );
                CVector2D vecSize ( pSector->m_vecTopRight.fX - pSector->m_vecBottomLeft.fX, pSector->m_vecTopRight.fY - pSector->m_vecBottomLeft.fY );
                pSector->m_pArea->SetSize ( vecSize );
                pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
            }
            pSector->m_pArea->SetColor ( 255, 0, 0, 50 );
        }
    }
    */

    // Has our position changed?
    if ( vecPosition != m_vecSectorPosition )
    {
        m_vecSectorPosition = vecPosition;

        // Have we changed row?
        if ( !m_pRow->DoesContain ( vecPosition ) )
        {
            m_pRow = FindOrCreateRow ( vecPosition, m_pRow );
            OnEnterSector ( m_pRow->FindOrCreateSector ( vecPosition ) );
        }
        // Have we changed sector?
        else if ( !m_pSector->DoesContain ( vecPosition ) )
        {
            // Grab our new sector
            OnEnterSector ( m_pRow->FindOrCreateSector ( vecPosition, m_pSector ) );
        }
    }
}


void CClientStreamer::OnEnterSector ( CClientStreamSector * pSector )
{
    // Grab a list of the sectors we want to activate/decactivate
    vector < CClientStreamSector * > activateSectors, deactivateSectors;

    if ( m_pSector )
    {                
        // Flag the sectors we want to save deactivating/reactivating the same ones.
        CClientStreamSector * pDeactivateSectors [ 9 ];
        m_pSector->GetSurroundingSectors ( pDeactivateSectors );
        pDeactivateSectors [ 8 ] = m_pSector;
        for ( int i = 0 ; i < 9 ; i++ ) pDeactivateSectors[i]->SetShouldBeActive ( false );

        CClientStreamSector * pActivateSectors [ 9 ];
        pSector->GetSurroundingSectors ( pActivateSectors );
        pActivateSectors [ 8 ] = pSector;
        for ( int i = 0 ; i < 9 ; i++ ) pActivateSectors[i]->SetShouldBeActive ( true );

        // Sort out our sectors
        CClientStreamSector * pTempSector = NULL;
        for ( int i = 0 ; i < 9 ; i++ )
        {
            // Deactivate the unwanted ones
            pTempSector = pDeactivateSectors[i];
            if ( pTempSector && !pTempSector->ShouldBeActive () && pTempSector->IsActivated () )
            {
                pTempSector->SetActivated ( false );
                deactivateSectors.push_back ( pTempSector );               
            }

            // activate the wanted ones
            pTempSector = pActivateSectors[i];
            if ( pTempSector && pTempSector->ShouldBeActive () && !pTempSector->IsActivated () )
            {
                pTempSector->SetActivated ( true );
                activateSectors.push_back ( pTempSector );
            }
        }
    }
    else
    {
        CClientStreamSector * pActivateSectors [ 9 ];
        pSector->GetSurroundingSectors ( pActivateSectors );
        pActivateSectors [ 8 ] = pSector;
        for ( int i = 0 ; i < 9 ; i++ ) 
        {
            if ( pActivateSectors [ i ] ) activateSectors.push_back ( pActivateSectors [ i ] );
        }
    }
    m_pSector = pSector;

    // Update all our sectors
    vector < CClientStreamer * > ::iterator iter = m_Streamers.begin ();
    for ( ; iter != m_Streamers.end () ; iter++ )
    {
        (*iter)->OnSectorChange ( activateSectors, deactivateSectors );
    }
}


void CClientStreamer::DoPulse ( CVector & vecPosition )
{   
    // Has our position changed?
    if ( vecPosition != m_vecPosition )
    {
        m_vecPosition = vecPosition;

        // Collect new distances
        SetExpDistances ( &m_ActiveElements );

        // Sort our active elements by distance
        m_ActiveElements.sort ( CompareExpDistance );
    }
    Restream ();
}


void CClientStreamer::SetDimension ( unsigned short usDimension )
{
    // Different dimension than before?
    if ( usDimension != m_usDimension )
    {
        // Set the new dimension
        m_usDimension = usDimension;

        // That means all of the currently streamed in elements will have to
        // go. Unstream all elements that are streamed in.
        CClientStreamElement * pElement = NULL;
        list < CClientStreamElement * > ::iterator iter = m_ActiveElements.begin ();
        for ( ; iter != m_ActiveElements.end () ; iter++ )
        {
            pElement = *iter;
            if ( pElement->IsStreamedIn () )
            {
                // Unstream it
                m_ToStreamOut.push_back ( pElement );
            }
        }
    }
}


CClientStreamSectorRow * CClientStreamer::FindOrCreateRow ( CVector & vecPosition, CClientStreamSectorRow * pSurrounding )
{
    // Do we have a row to check around first?
    if ( pSurrounding )
    {
        // Check the above and below rows
        if ( pSurrounding->m_pTop && pSurrounding->m_pTop->DoesContain ( vecPosition ) )
            return pSurrounding->m_pTop;
        if ( pSurrounding->m_pBottom && pSurrounding->m_pBottom->DoesContain ( vecPosition ) )
            return pSurrounding->m_pBottom;
    }

    // Search through our main world rows
    CClientStreamSectorRow * pRow = NULL;
    vector < CClientStreamSectorRow * > ::iterator iter = m_WorldRows.begin ();
    for ( ; iter != m_WorldRows.end () ; iter++ )
    {
        pRow = *iter;
        if ( pRow->DoesContain ( vecPosition ) )
        {
            return pRow;
        }
    }
    
    // Search through our extra rows
    iter = m_ExtraRows.begin ();
    for ( ; iter != m_ExtraRows.end () ; iter++ )
    {
        pRow = *iter;
        if ( pRow->DoesContain ( vecPosition ) )
        {
            return pRow;
        }
    }
    // We need a new row, align it with the others
    float fBottom = float ( ( int ) ( vecPosition.fY / ROW_SIZE ) ) * ROW_SIZE;
    if ( vecPosition.fY < 0.0f ) fBottom -= ROW_SIZE;
    pRow = new CClientStreamSectorRow ( fBottom, fBottom + ROW_SIZE );
    ConnectRow ( pRow );
    pRow->SetExtra ( true );
    m_ExtraRows.push_back ( pRow );
    return pRow;
}


CClientStreamSectorRow * CClientStreamer::FindRow ( float fY )
{
    // Search through our main world rows
    CClientStreamSectorRow * pRow = NULL;
    vector < CClientStreamSectorRow * > ::iterator iter = m_WorldRows.begin ();
    for ( ; iter != m_WorldRows.end () ; iter++ )
    {
        pRow = *iter;
        if ( pRow->DoesContain ( fY ) )
        {
            return pRow;
        }
    }
    
    // Search through our extra rows
    iter = m_ExtraRows.begin ();
    for ( ; iter != m_ExtraRows.end () ; iter++ )
    {
        pRow = *iter;
        if ( pRow->DoesContain ( fY ) )
        {
            return pRow;
        }
    }
    return NULL;
}


void CClientStreamer::OnUpdateStreamPosition ( CClientStreamElement * pElement )
{
    CVector vecPosition = pElement->GetStreamPosition ();    
    CClientStreamSectorRow * pRow = pElement->GetStreamRow ();
    CClientStreamSector * pSector = pElement->GetStreamSector ();

    // Have we changed row?
    if ( !pRow->DoesContain ( vecPosition ) )
    {
        pRow = FindOrCreateRow ( vecPosition );
        pElement->SetStreamRow ( pRow );
        OnElementEnterSector ( pElement, pRow->FindOrCreateSector ( vecPosition ) );
    }
    // Have we changed sector?
    else if ( !pSector->DoesContain ( vecPosition ) )
    {
        // Grab our new sector
        OnElementEnterSector ( pElement, pRow->FindOrCreateSector ( vecPosition, pSector ) );
    }
    else
    {
        // Make sure our distance is updated
        pElement->SetExpDistance ( GetElementStreamDistanceSquared ( pElement, m_vecPosition ) );
    }
}


void CClientStreamer::AddElement ( CClientStreamElement * pElement )
{
    assert ( pAddingElement == NULL );
    pAddingElement = pElement;
    CVector vecPosition = pElement->GetStreamPosition ();
    CClientStreamSectorRow * pRow = FindOrCreateRow ( vecPosition );
    pElement->SetStreamRow ( pRow );
    OnElementEnterSector ( pElement, pRow->FindOrCreateSector ( vecPosition ) );
    pAddingElement = NULL;
}


void CClientStreamer::RemoveElement ( CClientStreamElement * pElement )
{
    OnElementEnterSector ( pElement, NULL );
    m_ActiveElements.remove ( pElement );
	m_ToStreamOut.remove ( pElement );
}


void CClientStreamer::SetExpDistances ( list < CClientStreamElement * > * pList )
{
    // Run through our list setting distances to world center
    CClientStreamElement * pElement = NULL;
    list < CClientStreamElement* >:: iterator iter = pList->begin ();
    for ( ; iter != pList->end (); iter++ )
    {
        pElement = *iter;
        // Set its distance ^ 2
        pElement->SetExpDistance ( GetElementStreamDistanceSquared ( pElement, m_vecPosition ) );
    }
}


void CClientStreamer::AddToSortedList ( list < CClientStreamElement* > * pList, CClientStreamElement * pElement )
{
    // Make sure it's exp distance is updated
    float fDistance = GetElementStreamDistanceSquared ( pElement, m_vecPosition );
    pElement->SetExpDistance ( fDistance );

    // Don't add if already in the list
    if ( ListContains ( *pList, pElement ) )
        return;

    // Search through our list. Add it behind the first item further away than this
    CClientStreamElement * pTemp = NULL;
    list < CClientStreamElement* > :: const_iterator iter = pList->begin (), iterEnd = pList->end ();
    for ( ; iter != iterEnd; iter++ )
    {
        pTemp = *iter;

        // Is it further than the one we add?
        if ( GetElementStreamDistanceSquared ( pElement, m_vecPosition ) > fDistance )
        {
            // Add it before here
            pList->insert ( iter, pElement );
            return;
        }
    }

    // We have no elements in the list, add it at the beginning
    pList->push_back ( pElement );
}


bool CClientStreamer::CompareExpDistance ( CClientStreamElement* p1, CClientStreamElement* p2 )
{
    return p1->GetExpDistance () < p2->GetExpDistance ();
}


bool CClientStreamer::IsActiveElement ( CClientStreamElement * pElement )
{
    return ListContains ( m_ActiveElements, pElement );
}


void CClientStreamer::Restream ( void )
{
    // Do we have any elements waiting to be streamed out?
    CClientStreamElement * pElement = NULL;
    while ( !m_ToStreamOut.empty () )
    {
        pElement = m_ToStreamOut.front ();
        // Make sure we have no stream-references
        if ( pElement->GetTotalStreamReferences () == 0 )
        {
            // Stream out 1 of them per frame
            pElement->InternalStreamOut ();
        }
        m_ToStreamOut.remove ( pElement );
    }

    CClientStreamElement * pFurthestStreamed = NULL;
    CClientStreamElement * pClosestStreamedOut = NULL;
    float fElementDistanceExp;
    bool bReachedLimit = ReachedLimit ();
    // Loop through our active elements list (they should be ordered closest to furthest)
    list < CClientStreamElement* > ::const_iterator iter = m_ActiveElements.begin (), iterEnd = m_ActiveElements.end ();
    for ( ; iter != iterEnd; iter++ )
    {
        pElement = *iter;
        fElementDistanceExp = pElement->GetExpDistance ();
        
        // Is this element streamed in?
        if ( pElement->IsStreamedIn () )
        {
            // Too far away? Use the threshold so we won't flicker load it if it's on the border moving.
            if ( fElementDistanceExp > m_fMaxDistanceThreshold )
            {
                // Unstream it
                m_ToStreamOut.push_back ( pElement );
            }
            else pFurthestStreamed = pElement;
        }
        else
        {
            // Same dimension as us?
            if ( pElement->GetDimension () == m_usDimension )
            {
                // Too far away? Stop here.
                if ( fElementDistanceExp > m_fMaxDistanceExp )
                    continue;

                // If attached and attached-to is streamed out, don't consider for streaming in
                CClientStreamElement* pAttachedTo = dynamic_cast< CClientStreamElement * >( pElement->GetAttachedTo() );
                if ( pAttachedTo && !pAttachedTo->IsStreamedIn() )
                    continue;

                // Not room to stream in more elements?
                if ( bReachedLimit )
                {                 
                    // Do we not have a closest streamed out element yet? set it to this
                    if ( !pClosestStreamedOut ) pClosestStreamedOut = pElement;                    
                }
                else
                {
                    // Stream in the new element. Don't do it instantly.
                    pElement->InternalStreamIn ( false );
                    bReachedLimit = ReachedLimit ();
                }
            }            
        }
    }

    // Stream in the closest elements if we're at the limit
    if ( bReachedLimit )
    {
        // Do we have furthest-streamed and closest-streamed-out elements
        if ( pFurthestStreamed && pClosestStreamedOut )
        {
			// It isn't the same object
			if ( pFurthestStreamed != pClosestStreamedOut )
			{
				// Is the streamed out element closer?
				if ( pClosestStreamedOut->GetExpDistance () < pFurthestStreamed->GetExpDistance () )
				{   
					// Unstream the one furthest away
					pFurthestStreamed->InternalStreamOut();

					// Room to stream in element now? Stream it in.
					if ( !ReachedLimit () )
					{
						// Stream in the new element. No need to do it instantly.
						pClosestStreamedOut->InternalStreamIn ( false );
					}
				}
			}
        }
    }
}


void CClientStreamer::OnSectorChange ( vector < CClientStreamSector * > & activateSectors, vector < CClientStreamSector * > & deactivateSectors )
{
    CClientStreamSector * pSector;
    CClientStreamSectorList * pSectorList;
    CClientStreamElement * pElement;
    
    // First, stream out all the elements in deactive sectors
    vector < CClientStreamSector * > ::iterator iter = deactivateSectors.begin ();
    for ( ; iter != deactivateSectors.end () ; iter++ )
    {
        pSector = *iter;
        pSectorList = pSector->GetList ( this );

        list < CClientStreamElement * > ::iterator iterElements = pSectorList->Begin ();
        for ( ; iterElements != pSectorList->End () ; iterElements++ )
        {
            pElement = *iterElements;
            if ( pElement->IsStreamedIn () ) m_ToStreamOut.push_back ( pElement );
        }
    }

    // ..add all the elements in active sectors to our active-list
    iter = activateSectors.begin ();
    for ( ; iter != activateSectors.end () ; iter++ )
    {
        pSector = *iter;
        pSectorList = pSector->GetList ( this );

        pSectorList->AddElements ( &m_ActiveElements );
    }
    SetExpDistances ( &m_ActiveElements );
    m_ActiveElements.sort ( CompareExpDistance );
}


void CClientStreamer::OnElementEnterSector ( CClientStreamElement * pElement, CClientStreamSector * pSector )
{
    CClientStreamSectorList * pSectorList = NULL;
    CClientStreamSector * pPreviousSector = pElement->GetStreamSector ();
    if ( pPreviousSector )
    {
        // Remove the element from its old sector
        pSectorList = pPreviousSector->GetList ( this );
        pSectorList->Remove ( pElement );
    }
    if ( pSector )
    {
        pSectorList = pSector->GetList ( this );

        // Add the element to its new sector
        pSectorList->Add ( pElement );

        // Is this new sector activated?
        if ( pSector->IsActivated () )
        {
            // Was the previous sector not active?
            if ( !pPreviousSector || !pPreviousSector->IsActivated () )
            {
                // Add this element to our active-elements list
                AddToSortedList ( &m_ActiveElements, pElement );
            }            
        }
        else
        {
            // Should we activate this sector?
            if ( pSector->IsExtra () && ( m_pSector->IsMySurroundingSector ( pSector ) || m_pSector == pSector ) )
            {
                pSectorList->AddElements ( &m_ActiveElements );
                pSector->SetActivated ( true );
            }
            // If we're in a deactivated sector and streamed in, stream us out
            else if ( pElement->IsStreamedIn () )
            {
                m_ToStreamOut.push_back ( pElement );                
            }
        }        
    }
    pElement->SetStreamSector ( pSector );
}
    

void CClientStreamer::OnElementForceStreamIn ( CClientStreamElement * pElement )
{
    // Make sure we're streamed in
    pElement->InternalStreamIn ( true );
}


void CClientStreamer::OnElementForceStreamOut ( CClientStreamElement * pElement )
{
    // Make sure we're streamed out if need be
    if ( !IsActiveElement ( pElement ) )
    {
        m_ToStreamOut.push_back ( pElement );
    }
}


void CClientStreamer::OnElementDimension ( CClientStreamElement * pElement )
{
    // Grab its new dimenson
    unsigned short usDimension = pElement->GetDimension ();
    // Is it streamed in?
    if ( pElement->IsStreamedIn () )
    {
        // Has it moved to a different dimension to us?
        if ( usDimension != m_usDimension )
        {
            // Stream it out
            m_ToStreamOut.push_back ( pElement );
        }
    }
}