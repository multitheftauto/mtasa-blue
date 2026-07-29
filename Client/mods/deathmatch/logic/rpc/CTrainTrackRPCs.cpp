/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CTrainTrackRPCs.cpp
 *  PURPOSE:     Train track remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CTrainTrackRPCs.h"

void CTrainTrackRPCs::LoadFunctions()
{
    AddHandler(SET_TRAIN_TRACK_NODE_POSITION, SetTrainTrackNodePosition, "SetTrainTrackNodePosition");
}

void CTrainTrackRPCs::SetTrainTrackNodePosition(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    uint    nodeIndex;
    CVector position;
    if (bitStream.Read(nodeIndex) && bitStream.Read(position.fX) && bitStream.Read(position.fY) && bitStream.Read(position.fZ))
    {
        CClientTrainTrack* pTrainTrack = m_pTrainTrackManager->Get(pSource->GetID());
        if (pTrainTrack)
        {
            pTrainTrack->SetNodePosition(nodeIndex, position);
        }
    }
}
