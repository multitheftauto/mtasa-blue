/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTrainTrack.h>

class CTrainTrackSA final : public CTrainTrack
{
public:
    CTrainTrackSA(std::uint8_t trackID, const std::vector<CVector>& nodePositions, bool bLinkLastNodes);

    std::uint8_t                          GetTrackID() const noexcept override { return m_TrackID; }
    const std::vector<STrainTrackNodeSA>& GetNodes() const noexcept override { return m_Nodes; }
    float                                 GetTotalLength() const noexcept override { return m_fTotalLength; }
    bool                                  GetLastNodesLinked() const noexcept override { return m_bLinkLastNodes; }

private:
    std::uint8_t                   m_TrackID;
    std::vector<STrainTrackNodeSA> m_Nodes;
    float                          m_fTotalLength;
    bool                           m_bLinkLastNodes;
};
