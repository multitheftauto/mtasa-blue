/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTrainTrackManager.h>
#include "CTrainTrackSA.h"
#include <array>
#include <memory>

class CTrainTrackManagerSA final : public CTrainTrackManager
{
public:
    static constexpr std::size_t MAX_CUSTOM_TRACKS = 251;

    CTrainTrack* CreateTrainTrack(const std::vector<CVector>& nodePositions, bool bLinkLastNodes) override;
    void         DestroyTrainTrack(std::uint8_t trackID) override;
    CTrainTrack* GetTrainTrack(std::uint8_t trackID) override;

private:
    std::array<std::unique_ptr<CTrainTrackSA>, MAX_CUSTOM_TRACKS> m_Tracks;
};
