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

    // The track the game's own train code drives on. It comes back nullptr when there are no track
    // ID slots left, in which case no train can be placed on this element.
    m_pGameTrainTrack = g_pGame->GetTrainTrackManager()->CreateTrainTrack(nodePositions);

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
