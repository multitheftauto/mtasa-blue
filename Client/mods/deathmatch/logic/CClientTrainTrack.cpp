/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTrainTrack.cpp
*  PURPOSE:     Train Track class
*  DEVELOPERS:  Cazomino05 <>
*
*****************************************************************************/

#include <StdInc.h>

CClientTrainTrack::CClientTrainTrack ( class CClientManager* pManager, ElementID ID, unsigned int uiNodes, unsigned char ucTrackID, bool bLinkedLastNode ) : CClientEntity ( ID )
{
    // Init
    SetTypeName ( "train-track" );
    m_pTrainTrack = g_pGame->GetTrainTrackManager ( )->CreateTrainTrack ( uiNodes, ucTrackID, bLinkedLastNode );
}

CClientTrainTrack::~CClientTrainTrack ( void )
{
    g_pGame->GetTrainTrackManager ( )->DestroyTrainTrack ( m_pTrainTrack->GetTrackID ( ) );
    m_pTrainTrack = NULL;
}

bool CClientTrainTrack::SetNodePosition ( unsigned int uiNode, CVector vecPosition )
{
    return m_pTrainTrack->SetRailNodePosition ( uiNode, vecPosition );
}

bool CClientTrainTrack::GetNodePosition ( unsigned int uiNode, CVector& vecPosition )
{
    return m_pTrainTrack->GetRailNodePosition ( uiNode, vecPosition );
}

bool CClientTrainTrack::SetTrackLength ( float fLength )
{
    return m_pTrainTrack->SetTrackLength ( fLength );
}

float CClientTrainTrack::GetTrackLength ( void )
{
    return m_pTrainTrack->GetTrackLength ( );
}

bool CClientTrainTrack::SetNumberOfNodes ( unsigned int uiNodes )
{
    return m_pTrainTrack->SetNumberOfNodes ( uiNodes );
}

unsigned int CClientTrainTrack::GetNumberOfNodes ( void )
{
    return m_pTrainTrack->GetNumberOfNodes ( );
}

unsigned char CClientTrainTrack::GetTrackID ( void )
{
    return m_pTrainTrack->GetTrackID ( );
}

bool CClientTrainTrack::SetLastNodesLinked ( bool bLinked )
{
    m_pTrainTrack->SetLastNodesLinked ( bLinked );
    return true;
}