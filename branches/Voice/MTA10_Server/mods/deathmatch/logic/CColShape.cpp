/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CColShape.cpp
*  PURPOSE:     Base shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CColShape::CColShape ( CColManager* pManager, CElement* pParent, CXMLNode* pNode, bool bIsPartnered ) : CElement ( pParent, pNode )
{
    // Init
    m_bIsEnabled = true;
    m_bAutoCallEvent = true;
    m_pCallback = NULL;
    m_iType = COLSHAPE;
    m_bPartnered = bIsPartnered;

    SetTypeName ( "colshape" );

    // Add it to our manager's list
    m_pManager = pManager;
    pManager->AddToList ( this );
}


CColShape::~CColShape ( void )
{
    if ( m_pCallback )
        m_pCallback->Callback_OnCollisionDestroy ( this );

    RemoveAllColliders ( true );
    // Unlink us from our manager
    Unlink ();
}


void CColShape::Unlink ( void )
{
    // Remove us from manager's list
    m_pManager->RemoveFromList ( this );
}


const CVector & CColShape::GetPosition ( void )
{
    if ( m_pAttachedTo ) GetAttachedPosition ( m_vecPosition );
    return m_vecPosition;
}


void CColShape::SetPosition ( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
    UpdateSpatialData ();
    CStaticFunctionDefinitions::RefreshColShapeColliders ( this );
}


void CColShape::CallHitCallback ( CElement& Element )
{
    // Call the callback with us as the shape if it exists
    if ( m_pCallback )
    {
        m_pCallback->Callback_OnCollision ( *this, Element );
    }
}


void CColShape::CallLeaveCallback ( CElement& Element )
{
    // Call the callback with us as the shape if it exists
    if ( m_pCallback )
    {
        m_pCallback->Callback_OnLeave ( *this, Element );
    }
}


bool CColShape::ColliderExists ( CElement* pElement )
{
    list < CElement* > ::iterator iter = m_Colliders.begin ();
    for ( ; iter != m_Colliders.end () ; iter++ )
    {
        if ( *iter == pElement )
        {
            return true;
        }
    }    
    return false;
}


void CColShape::RemoveAllColliders ( bool bNotify )
{
    if ( bNotify )
    {
        list < CElement* > ::iterator iter = m_Colliders.begin ();
        for ( ; iter != m_Colliders.end () ; iter++ )
        {
            (*iter)->RemoveCollision ( this );
        }
    }
    m_Colliders.clear ();
}


void CColShape::SizeChanged ( void )
{
    UpdateSpatialData ();
    // Maybe queue RefreshColliders for v1.1
}
