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
    // Track IDs are a byte and 0xFF means "no track", so 0-254 are usable
    static constexpr std::size_t MAX_TRACKS = 255;
    static constexpr std::size_t MAX_CUSTOM_TRACKS = MAX_TRACKS - FIRST_CUSTOM_TRACK_ID;

    CTrainTrackManagerSA();

    CTrainTrack* CreateTrainTrack(const std::vector<CVector>& nodePositions, bool bLinkLastNodes) override;
    void         DestroyTrainTrack(std::uint8_t trackID) override;
    CTrainTrack* GetTrainTrack(std::uint8_t trackID) override;
    float        GetTrackLength(std::uint8_t trackID) const noexcept override;

    // Anything that used to read the game's own track globals has to come through here instead,
    // since after the relocation the originals are never populated
    static SRailNodeSA* GetTrackNodes(std::uint8_t trackID) noexcept;
    static std::int32_t GetTrackNodeCount(std::uint8_t trackID) noexcept;

    // The game's three per-track globals, relocated so a track ID past its built-in 4 stays in bounds.
    // CTrainTrackSA writes its own entry here; nothing else should touch them.
    static void SetTrackData(std::uint8_t trackID, SRailNodeSA* pNodes, std::int32_t numNodes, float fTotalLength) noexcept;

private:
    // Repoints every instruction that indexes the built-in track arrays at the ones above
    static bool RelocateTrackArrays() noexcept;

    static SRailNodeSA* ms_pTrackNodes[MAX_TRACKS];
    static std::int32_t ms_NumTrackNodes[MAX_TRACKS];
    static float        ms_TotalTrackLength[MAX_TRACKS];

    static bool ms_bArraysRelocated;

    std::array<std::unique_ptr<CTrainTrackSA>, MAX_CUSTOM_TRACKS> m_Tracks;
};
