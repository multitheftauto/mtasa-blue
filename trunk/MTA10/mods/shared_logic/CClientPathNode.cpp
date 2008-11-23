/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPathNode.cpp
*  PURPOSE:     Path node entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

CClientPathNode::CClientPathNode ( CClientManager* pManager, CVector& vecPosition,
                                   CVector& vecRotation, int iTime, ElementID ID, ePathNodeStyle PathStyle,
                                   CClientPathNode* pNextNode ) : CClientEntity ( ID )
{
    m_pManager = pManager;
    m_pPathManager = pManager->GetPathManager ();

    m_vecPosition = vecPosition;
    m_vecRotation = vecRotation;
    m_iTime = iTime;
    m_Style = PathStyle;
    m_NextNodeID = INVALID_ELEMENT_ID;
    m_pPreviousNode = NULL;
    m_pNextNode = pNextNode;

    m_pPathManager->AddToList ( this );
}


CClientPathNode::~CClientPathNode ( void )
{
    Unlink ();

    if ( m_pPreviousNode )
        m_pPreviousNode->SetNextNode ( NULL );
    if ( m_pNextNode )
        m_pNextNode->SetPreviousNode ( NULL );
}


void CClientPathNode::Unlink ( void )
{
    m_pPathManager->RemoveFromList ( this );
}


void CClientPathNode::DoPulse ( void )
{
    list < CClientEntity* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // TEST!
        if ( (*iter)->GetType () == CCLIENTVEHICLE )
        {
            CClientVehicle* pVehicle = static_cast < CClientVehicle* > ( *iter );
            CVector vecMoveSpeed;
            pVehicle->GetMoveSpeed ( vecMoveSpeed );
            vecMoveSpeed.fZ = 0.0f;
            pVehicle->SetMoveSpeed ( vecMoveSpeed );   
        }
        (*iter)->SetPosition ( m_vecPosition );        
    }
}

bool CClientPathNode::IsEntityAttached ( CClientEntity* pEntity )
{
    list < CClientEntity* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        if ( *iter == pEntity )
            return true;
    }

    return false;
}


