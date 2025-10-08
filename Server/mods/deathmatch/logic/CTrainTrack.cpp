/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainTrack.h"
#include "CTrainTrackManager.h"
#include "CVehicleManager.h"
#include "CGame.h"

CTrainTrack::CTrainTrack(CTrainTrackManager* pManager, const std::vector<STrackNode>& nodes, bool linkLastNodes, CElement* pParent, uchar defaultTrackId)
    : CElement(pParent), m_pManager(pManager)
{
    m_iType = CElement::TRAIN_TRACK;
    SetTypeName("train-track");

    m_LinkLastNodes = linkLastNodes;
    m_Nodes = nodes;
    m_DefaultTrackId = defaultTrackId;
}

CTrainTrack::~CTrainTrack()
{
    // Remove all vehicles from the track
    CVehicleManager* pVehicleManager = g_pGame->GetVehicleManager();
    for (auto pVehicle : pVehicleManager->GetVehicles())
    {
        if (pVehicle->GetTrainTrack() != this)
            continue;

        pVehicle->SetTrainTrack(nullptr);
        pVehicle->SetDerailed(true);
    }

    // Unreference train track
    m_pManager->DestroyTrainTrack(this);
}

bool CTrainTrack::SetTrackNodePosition(uint nodeIndex, const CVector& position)
{
    if (nodeIndex >= m_Nodes.size())
        return false;

    auto& node = m_Nodes[nodeIndex];
    node.position = position;
    return true;
}

bool CTrainTrack::GetTrackNodePosition(uint nodeIndex, CVector& position)
{
    if (nodeIndex >= m_Nodes.size())
        return false;

    auto& node = m_Nodes[nodeIndex];
    position = node.position;
    return true;
}
