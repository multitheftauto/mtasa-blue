/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamElement.cpp
*  PURPOSE:     Streamed entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientStreamElement::CClientStreamElement ( CClientStreamer * pStreamer, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    m_pStreamer = pStreamer;    
    m_pStreamRow = NULL;
    m_pStreamSector = NULL;
    m_fExpDistance = 0.0f;
    m_bStreamedIn = false;
    m_bAttemptingToStreamIn = false;
    m_usStreamReferences = 0; m_usStreamReferencesScript = 0;
    m_pStreamer->AddElement ( this );

    m_fCachedRadius = 0;
    m_iCachedRadiusCounter = 0;
    m_iCachedBoundingBoxCounter = 0;
}


CClientStreamElement::~CClientStreamElement ( void )
{
    m_pStreamer->RemoveElement ( this );
}


void CClientStreamElement::UpdateStreamPosition ( const CVector & vecPosition )
{
    m_vecStreamPosition = vecPosition;    
    UpdateSpatialData ();
    m_pStreamer->OnUpdateStreamPosition ( this );
    m_pManager->OnUpdateStreamPosition ( this );

    // Update attached elements stream position
    list < CClientEntity* >::iterator i = m_AttachedEntities.begin();
    for (; i != m_AttachedEntities.end(); i++)
    {
        CClientStreamElement* attachedElement = dynamic_cast< CClientStreamElement* > (*i);
        if ( attachedElement )
        {
            attachedElement->UpdateStreamPosition( vecPosition + attachedElement->m_vecAttachedPosition );
        }
    }
}


void CClientStreamElement::InternalStreamIn ( bool bInstantly )
{
    if ( !m_bStreamedIn && !m_bAttemptingToStreamIn )
    {
        m_bAttemptingToStreamIn = true;
        StreamIn ( bInstantly );        
    }
}


void CClientStreamElement::InternalStreamOut ( void )
{
    if ( m_bStreamedIn )
    {
        StreamOut ();
        m_bStreamedIn = false;

        // Stream out attached elements
        list < CClientEntity* >::iterator i = m_AttachedEntities.begin();
        for (; i != m_AttachedEntities.end(); i++)
        {
            CClientStreamElement* attachedElement = dynamic_cast< CClientStreamElement* > (*i);
            if ( attachedElement )
            {
                // Don't stream out low LOD version
                CClientObject* attachedObject = DynamicCast < CClientObject > (*i);
                if ( attachedObject->IsLowLod () )
                    continue;
                attachedElement->InternalStreamOut();
            }
        }

        CLuaArguments Arguments;
        CallEvent ( "onClientElementStreamOut", Arguments, true );
    }
}


void CClientStreamElement::NotifyCreate ( void )
{
    // Update common atrributes
    if ( !m_bDoubleSidedInit )
        m_bDoubleSided = IsDoubleSided ();
    SetDoubleSided ( m_bDoubleSided );

    m_bStreamedIn = true;
    m_bAttemptingToStreamIn = false;

    CLuaArguments Arguments;
    CallEvent ( "onClientElementStreamIn", Arguments, true );
}


void CClientStreamElement::NotifyUnableToCreate ( void )
{
    m_bAttemptingToStreamIn = false;
}


void CClientStreamElement::AddStreamReference ( bool bScript )
{
    unsigned short * pRefs = ( bScript ) ? &m_usStreamReferencesScript : &m_usStreamReferences;
    if ( (*pRefs) < 0xFFFF ) (*pRefs)++;

    // Have we added the first reference?
    if ( ( m_usStreamReferences + m_usStreamReferencesScript ) == 1 )
    {
        m_pStreamer->OnElementForceStreamIn ( this );
    }
}


void CClientStreamElement::RemoveStreamReference ( bool bScript )
{
    unsigned short * pRefs = ( bScript ) ? &m_usStreamReferencesScript : &m_usStreamReferences;
    if ( (*pRefs) > 0 )
    {    
        (*pRefs)--;
    }

    // Have we removed the last reference?
    if ( ( m_usStreamReferences + m_usStreamReferencesScript ) == 0 )
    {
        m_pStreamer->OnElementForceStreamOut ( this );
    }
}


unsigned short CClientStreamElement::GetStreamReferences ( bool bScript )
{
    unsigned short * pRefs = ( bScript ) ? &m_usStreamReferencesScript : &m_usStreamReferences;
    return (*pRefs);
}

// Force the element to stream out now. It will stream back in next frame if close enough.
void CClientStreamElement::StreamOutForABit ( void )
{
    // Remove asap, very messy
    InternalStreamOut ();
}


void CClientStreamElement::SetDimension ( unsigned short usDimension )
{
    // Different dimension than before?
    if ( m_usDimension != usDimension )
    {
        // Set the new dimension
        m_usDimension = usDimension;

        m_pStreamer->OnElementDimension ( this );
    }
}


CSphere CClientStreamElement::GetWorldBoundingSphere ( void )
{
    // Default to a point at stream position
    return CSphere ( GetStreamPosition (), 0.0f );
}


// Helper function for CClientStreamElement::GetDistanceToBoundingBoxSquared()
static float GetBoxDistanceSq ( const CVector& vecPosition, const CVector& vecBoxCenter, const float* fExtentMin, const float* fExtentMax, const CVector** vecBoxAxes )
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


float CClientStreamElement::GetDistanceToBoundingBoxSquared ( const CVector& vecPosition )
{
    // Do a simple calculation if the element is newly added ( hack/fix for CClientSteamer::AddElement being called in the CClientStreamElement constructor )
    if ( this == CClientStreamer::pAddingElement )
    {
        return ( GetStreamPosition () - vecPosition ).LengthSquared ();
    }

    // More hax to increase performance

    // Update cached radius if required
    if ( --m_iCachedRadiusCounter < 0 )
    {
        CStaticFunctionDefinitions::GetElementRadius ( *this, m_fCachedRadius );
        m_iCachedRadiusCounter = 20 + rand() % 50;
    }

    // Do a simple calculation if the element has a small radius
    if ( m_fCachedRadius < 20 )
    {
        return ( GetStreamPosition () - vecPosition ).LengthSquared ();
    }

    // Update cached bounding box if required
    if ( --m_iCachedBoundingBoxCounter < 0 )
    {
        // Get bounding box extents
        CVector vecMin;
        CVector vecMax;
        CStaticFunctionDefinitions::GetElementBoundingBox ( *this, vecMin, vecMax );

        // Adjust for non-centered bounding box
        CVector vecHalfCenter = ( vecMin + vecMax ) * 0.25f;
        vecMin -= vecHalfCenter;
        vecMax -= vecHalfCenter;

        m_vecCachedBoundingBox[0] = vecMin;
        m_vecCachedBoundingBox[1] = vecMax;

        m_iCachedBoundingBoxCounter = 20 + rand() % 50;
    }

    const CVector& vecMin = m_vecCachedBoundingBox[0];
    const CVector& vecMax = m_vecCachedBoundingBox[1];

    // Get bounding box axes
    CMatrix gtaMatrix;
    GetMatrix ( gtaMatrix );

    const CVector* vecBoxAxes[3] = { &gtaMatrix.vRight, &gtaMatrix.vFront, &gtaMatrix.vUp };

    return GetBoxDistanceSq ( vecPosition, GetStreamPosition (), &vecMin.fX, &vecMax.fX, vecBoxAxes );
}
