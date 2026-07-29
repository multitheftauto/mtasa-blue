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
#include <game/CTrainTrackManager.h>
#include <game/CTrainTrack.h>

CClientTrainTrack::CClientTrainTrack(CClientManager* pManager, ElementID ID, const std::vector<CVector>& nodePositions, bool bLinkLastNodes)
    : ClassInit(this), CClientEntity(ID)
{
    m_pManager = pManager;
    m_pTrainTrackManager = pManager->GetTrainTrackManager();

    SetTypeName("train-track");

    m_NodePositions = nodePositions;
    m_bLinkLastNodes = bLinkLastNodes;
    m_vecPosition = nodePositions.empty() ? CVector() : nodePositions.front();

    // This is what the game's native train code actually drives on; if we run out of track ID
    // slots it comes back nullptr and the track just won't move any train placed on it
    m_pGameTrainTrack = g_pGame->GetTrainTrackManager()->CreateTrainTrack(nodePositions, bLinkLastNodes);

    m_pTrainTrackManager->AddToList(this);
}

CClientTrainTrack::~CClientTrainTrack()
{
    if (m_pGameTrainTrack)
        g_pGame->GetTrainTrackManager()->DestroyTrainTrack(m_pGameTrainTrack->GetTrackID());

    Unlink();
}

std::uint8_t CClientTrainTrack::GetGameTrackID() const noexcept
{
    return m_pGameTrainTrack ? m_pGameTrainTrack->GetTrackID() : 0xFF;
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

    // Keep the data the movement hook actually reads in sync too
    if (m_pGameTrainTrack)
        m_pGameTrainTrack->SetNodePosition(static_cast<std::uint32_t>(nodeIndex), position);

    return true;
}
