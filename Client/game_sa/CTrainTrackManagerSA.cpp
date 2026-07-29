/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainTrackManagerSA.h"
#include <cstring>

namespace
{
    ////////////////////////////////////////////////////////////////////////
    // The game keeps three per-track globals, each a fixed array of 4 entries indexed by the track ID
    // a train carries. Nothing bounds-checks that index, so CTrain::ProcessControl and friends read
    // past the end of them for any ID beyond the built-in 4.
    //
    // Every instruction that indexes those globals is pointed at larger arrays instead, leaving the
    // train code itself untouched. A custom track is therefore driven by the game's own routines and
    // gets their engine audio, brake sounds, station logic and derailing for free.
    ////////////////////////////////////////////////////////////////////////
    constexpr std::uint32_t GAME_TOTAL_TRACK_LENGTH = ARRAY_RailTrackLengths;
    constexpr std::uint32_t GAME_NUM_TRACK_NODES = ARRAY_NumRailTrackNodes;
    constexpr std::uint32_t GAME_TRACK_NODES = ARRAY_RailTrackNodePointers;

    constexpr std::uint32_t GAME_ARRAY_BYTES = NUM_RAILTRACKS * sizeof(std::uint32_t);

    // Every operand in the train code holding one of those three base addresses, found by scanning
    // gta_sa.exe for dword references into them. All are disp32/imm32 fields, so rewriting them in
    // place leaves every instruction exactly the same length and needs no trampolines.
    constexpr std::uint32_t TRACK_ARRAY_OPERANDS[] = {
        0x6F58D2, 0x6F59EB, 0x6F59FE, 0x6F5BC4, 0x6F5BD9, 0x6F5C15, 0x6F5C32, 0x6F6BF3, 0x6F6C06, 0x6F6C96, 0x6F6CD1, 0x6F6D04, 0x6F6D0A,
        0x6F6EB1, 0x6F6F52, 0x6F6F79, 0x6F6FE8, 0x6F7442, 0x6F745D, 0x6F7462, 0x6F7467, 0x6F747A, 0x6F7483, 0x6F7488, 0x6F748D, 0x6F74A0,
        0x6F74A9, 0x6F74AE, 0x6F74B3, 0x6F74C6, 0x6F74CF, 0x6F74D4, 0x6F74D9, 0x6F74EC, 0x6F75B7, 0x6F7B73, 0x6F7C60, 0x6F7DB6, 0x6F7DC5,
        0x6F7EE9, 0x6F7F05, 0x6F7F12, 0x6F8001, 0x6F8007, 0x6F809C, 0x6F80B6, 0x6F80C3, 0x6F8712, 0x6F871C, 0x6F8723,
    };

    // Maps an operand's current value onto the matching entry of a relocated array
    bool ResolveOperand(std::uint32_t value, std::uint32_t& outOldBase, const void*& outNewBase, const SRailNodeSA* const* pNodes,
                        const std::int32_t* pNumNodes, const float* pTotalLength) noexcept
    {
        if (value - GAME_TOTAL_TRACK_LENGTH < GAME_ARRAY_BYTES)
        {
            outOldBase = GAME_TOTAL_TRACK_LENGTH;
            outNewBase = pTotalLength;
            return true;
        }

        if (value - GAME_NUM_TRACK_NODES < GAME_ARRAY_BYTES)
        {
            outOldBase = GAME_NUM_TRACK_NODES;
            outNewBase = pNumNodes;
            return true;
        }

        if (value - GAME_TRACK_NODES < GAME_ARRAY_BYTES)
        {
            outOldBase = GAME_TRACK_NODES;
            outNewBase = pNodes;
            return true;
        }

        return false;
    }
}  // namespace

SRailNodeSA* CTrainTrackManagerSA::ms_pTrackNodes[MAX_TRACKS] = {};
std::int32_t CTrainTrackManagerSA::ms_NumTrackNodes[MAX_TRACKS] = {};
float        CTrainTrackManagerSA::ms_TotalTrackLength[MAX_TRACKS] = {};
bool         CTrainTrackManagerSA::ms_bArraysRelocated = false;

CTrainTrackManagerSA::CTrainTrackManagerSA()
{
    RelocateTrackArrays();
}

bool CTrainTrackManagerSA::RelocateTrackArrays() noexcept
{
    if (ms_bArraysRelocated)
        return true;

    // Check every site before touching any of them, so an unexpected binary leaves the game exactly
    // as it was instead of running on half-patched train code
    for (std::uint32_t operandAddress : TRACK_ARRAY_OPERANDS)
    {
        std::uint32_t oldBase = 0;
        const void*   newBase = nullptr;
        if (!ResolveOperand(*reinterpret_cast<const std::uint32_t*>(operandAddress), oldBase, newBase, ms_pTrackNodes, ms_NumTrackNodes, ms_TotalTrackLength))
            return false;
    }

    // Seed the built-in entries from the game's arrays, which hold the track data whenever the game
    // gets to load it before this runs
    std::memcpy(ms_TotalTrackLength, reinterpret_cast<const void*>(GAME_TOTAL_TRACK_LENGTH), GAME_ARRAY_BYTES);
    std::memcpy(ms_NumTrackNodes, reinterpret_cast<const void*>(GAME_NUM_TRACK_NODES), GAME_ARRAY_BYTES);
    std::memcpy(ms_pTrackNodes, reinterpret_cast<const void*>(GAME_TRACK_NODES), GAME_ARRAY_BYTES);

    for (std::uint32_t operandAddress : TRACK_ARRAY_OPERANDS)
    {
        const std::uint32_t oldValue = *reinterpret_cast<const std::uint32_t*>(operandAddress);

        std::uint32_t oldBase = 0;
        const void*   newBase = nullptr;
        ResolveOperand(oldValue, oldBase, newBase, ms_pTrackNodes, ms_NumTrackNodes, ms_TotalTrackLength);

        MemPut<std::uint32_t>(operandAddress, reinterpret_cast<std::uint32_t>(newBase) + (oldValue - oldBase));
    }

    ms_bArraysRelocated = true;
    return true;
}

void CTrainTrackManagerSA::SetTrackData(std::uint8_t trackID, SRailNodeSA* pNodes, std::int32_t numNodes, float fTotalLength) noexcept
{
    if (!ms_bArraysRelocated || trackID >= MAX_TRACKS)
        return;

    ms_pTrackNodes[trackID] = pNodes;
    ms_NumTrackNodes[trackID] = numNodes;
    ms_TotalTrackLength[trackID] = fTotalLength;
}

CTrainTrack* CTrainTrackManagerSA::CreateTrainTrack(const std::vector<CVector>& nodePositions)
{
    // A custom track ID is only in bounds for the train code once the arrays are relocated, so
    // refuse to hand one out unless that succeeded
    if (!ms_bArraysRelocated || nodePositions.size() < 2)
        return nullptr;

    for (std::size_t i = 0; i < m_Tracks.size(); i++)
    {
        if (m_Tracks[i] != nullptr)
            continue;

        std::uint8_t trackID = static_cast<std::uint8_t>(FIRST_CUSTOM_TRACK_ID + i);
        m_Tracks[i] = std::make_unique<CTrainTrackSA>(trackID, nodePositions);
        return m_Tracks[i].get();
    }

    return nullptr;
}

void CTrainTrackManagerSA::DestroyTrainTrack(std::uint8_t trackID)
{
    if (trackID < FIRST_CUSTOM_TRACK_ID)
        return;

    std::size_t index = static_cast<std::size_t>(trackID) - FIRST_CUSTOM_TRACK_ID;
    if (index < m_Tracks.size())
        m_Tracks[index].reset();
}

CTrainTrack* CTrainTrackManagerSA::GetTrainTrack(std::uint8_t trackID)
{
    if (trackID < FIRST_CUSTOM_TRACK_ID)
        return nullptr;

    std::size_t index = static_cast<std::size_t>(trackID) - FIRST_CUSTOM_TRACK_ID;
    if (index >= m_Tracks.size())
        return nullptr;

    return m_Tracks[index].get();
}
