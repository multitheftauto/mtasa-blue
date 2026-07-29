/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTrainTrack.cpp
 *  PURPOSE:     Train track entity class
 *
 *****************************************************************************/

#include "StdInc.h"

CClientTrainTrack::CClientTrainTrack(CClientManager* pManager, ElementID ID, const std::vector<CVector>& nodePositions, bool bLinkLastNodes)
    : ClassInit(this), CClientEntity(ID)
{
    m_pManager = pManager;
    m_pTrainTrackManager = pManager->GetTrainTrackManager();

    SetTypeName("train-track");

    m_NodePositions = nodePositions;
    m_bLinkLastNodes = bLinkLastNodes;
    m_vecPosition = nodePositions.empty() ? CVector() : nodePositions.front();

    m_pTrainTrackManager->AddToList(this);
}

CClientTrainTrack::~CClientTrainTrack()
{
    Unlink();
}

void CClientTrainTrack::Unlink()
{
    m_pTrainTrackManager->RemoveFromList(this);
}

bool CClientTrainTrack::GetNodePosition(std::size_t nodeIndex, CVector& position) const
{
    if (nodeIndex >= m_NodePositions.size())
        return false;

    position = m_NodePositions[nodeIndex];
    return true;
}

bool CClientTrainTrack::SetNodePosition(std::size_t nodeIndex, const CVector& position)
{
    if (nodeIndex >= m_NodePositions.size())
        return false;

    m_NodePositions[nodeIndex] = position;
    return true;
}
