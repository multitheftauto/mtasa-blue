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
#include "CClientTrainTrack.h"

CClientTrainTrack::CClientTrainTrack(ElementID ID, const std::vector<STrackNode>& trackNodes, bool linkLastNode) : CClientEntity(ID)
{
    SetTypeName("train-track");
    m_pTrainTrack = g_pGame->GetTrainTrackManager()->CreateTrainTrack(trackNodes, linkLastNode);
}

CClientTrainTrack::~CClientTrainTrack()
{
    g_pGame->GetTrainTrackManager()->DestroyTrainTrack(m_pTrainTrack);
}

bool CClientTrainTrack::SetNodePosition(uint nodeIndex, const CVector& position)
{
    return m_pTrainTrack->SetNodePosition(nodeIndex, position);
}

bool CClientTrainTrack::GetNodePosition(uint nodeIndex, CVector& position)
{
    return m_pTrainTrack->GetNodePosition(nodeIndex, position);
}

float CClientTrainTrack::GetLength()
{
    return m_pTrainTrack->GetLength();
}

uint CClientTrainTrack::GetTrackIndex()
{
    return m_pTrainTrack->GetIndex();
}

size_t CClientTrainTrack::GetNumberOfNodes()
{
    return m_pTrainTrack->GetNodes().size();
}

void CClientTrainTrack::SetLastNodesLinked(bool linked)
{
    m_pTrainTrack->SetLastNodesLinked(linked);
}
