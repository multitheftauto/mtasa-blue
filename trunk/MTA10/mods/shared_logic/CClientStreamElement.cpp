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

CClientStreamElement::CClientStreamElement ( CClientStreamer * pStreamer, ElementID ID ) : CClientEntity ( ID )
{
    m_pStreamer = pStreamer;    
    m_pStreamRow = NULL;
    m_pStreamSector = NULL;
    m_fExpDistance = 0.0f;
    m_bStreamedIn = false;
    m_bAttemptingToStreamIn = false;
    m_usStreamReferences = 0; m_usStreamReferencesScript = 0;
    m_pStreamer->AddElement ( this );
}


CClientStreamElement::~CClientStreamElement ( void )
{
    m_pStreamer->RemoveElement ( this );
}


void CClientStreamElement::UpdateStreamPosition ( const CVector & vecPosition )
{
    m_vecStreamPosition = vecPosition;    
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
                attachedElement->StreamOut();
            }
        }

        CLuaArguments Arguments;
        CallEvent ( "onClientElementStreamOut", Arguments, true );
    }
}


void CClientStreamElement::NotifyCreate ( void )
{
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

        // Have we removed the last reference?
        if ( (*pRefs) == 0 )
        {
            m_pStreamer->OnElementForceStreamOut ( this );
        }
    }
}


unsigned short CClientStreamElement::GetStreamReferences ( bool bScript )
{
    unsigned short * pRefs = ( bScript ) ? &m_usStreamReferencesScript : &m_usStreamReferences;
    return (*pRefs);
}


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