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

    m_Nodes = nodes;
    m_LinkLastNodes = linkLastNodes;
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
