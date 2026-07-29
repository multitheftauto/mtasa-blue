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
#include "CVehicleSA.h"

class CTrainTrackSA final : public CTrainTrack
{
public:
    CTrainTrackSA(std::uint8_t trackID, const std::vector<CVector>& nodePositions, bool bLinkLastNodes);
    ~CTrainTrackSA();

    std::uint8_t                          GetTrackID() const noexcept override { return m_TrackID; }
    const std::vector<STrainTrackNodeSA>& GetNodes() const noexcept override { return m_Nodes; }
    float                                 GetTotalLength() const noexcept override { return m_fTotalLength; }
    bool                                  GetLastNodesLinked() const noexcept override { return m_bLinkLastNodes; }
    bool                                  SetNodePosition(std::uint32_t nodeIndex, const CVector& position) noexcept override;

private:
    void RecomputeDistances() noexcept;

    // Rebuilds the game-format node array and hands it to the train code
    void PublishToGame() noexcept;

    std::uint8_t                   m_TrackID;
    std::vector<STrainTrackNodeSA> m_Nodes;      // full precision, what scripts read back
    std::vector<SRailNodeSA>       m_GameNodes;  // quantised, what the game's train code walks
    float                          m_fTotalLength;
    bool                           m_bLinkLastNodes;
};
