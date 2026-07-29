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
    // The game keeps three per-track globals, each a fixed array of 4 entries indexed by the track
    // ID a train carries. Nothing bounds-checks that index, which is the only reason a track ID past
    // the built-in 4 could not be used: CTrain::ProcessControl and friends would simply read past the
    // end of these arrays.
    //
    // Rather than reimplementing the train code, we point every instruction that indexes them at our
    // own, larger arrays. The game then drives a custom track through its own untouched code, so the
    // engine audio, brake sounds, station logic and derailing all keep working.
    ////////////////////////////////////////////////////////////////////////
    constexpr std::uint32_t GAME_TOTAL_TRACK_LENGTH = 0xC37FEC;  // float[4]
    constexpr std::uint32_t GAME_NUM_TRACK_NODES = 0xC38014;     // std::int32_t[4]
    constexpr std::uint32_t GAME_TRACK_NODES = 0xC38024;         // SRailNodeSA*[4]

    constexpr std::uint32_t GAME_ARRAY_BYTES = 4 * sizeof(std::uint32_t);

    // Every operand in the train code holding one of those three base addresses, found by scanning
    // gta_sa.exe for dword references into them. All are disp32/imm32 fields, so rewriting them in
    // place leaves every instruction exactly the same length - no trampolines needed.
    constexpr std::uint32_t TRACK_ARRAY_OPERANDS[] = {
        0x6F58D2, 0x6F59EB, 0x6F59FE, 0x6F5BC4, 0x6F5BD9, 0x6F5C15, 0x6F5C32, 0x6F6BF3, 0x6F6C06, 0x6F6C96,
        0x6F6CD1, 0x6F6D04, 0x6F6D0A, 0x6F6EB1, 0x6F6F52, 0x6F6F79, 0x6F6FE8, 0x6F7442, 0x6F745D, 0x6F7462,
        0x6F7467, 0x6F747A, 0x6F7483, 0x6F7488, 0x6F748D, 0x6F74A0, 0x6F74A9, 0x6F74AE, 0x6F74B3, 0x6F74C6,
        0x6F74CF, 0x6F74D4, 0x6F74D9, 0x6F74EC, 0x6F75B7, 0x6F7B73, 0x6F7C60, 0x6F7DB6, 0x6F7DC5, 0x6F7EE9,
        0x6F7F05, 0x6F7F12, 0x6F8001, 0x6F8007, 0x6F809C, 0x6F80B6, 0x6F80C3, 0x6F8712, 0x6F871C, 0x6F8723,
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
        if (!ResolveOperand(*reinterpret_cast<const std::uint32_t*>(operandAddress), oldBase, newBase, ms_pTrackNodes, ms_NumTrackNodes,
                            ms_TotalTrackLength))
            return false;
    }

    // Carry over anything the game already loaded, in case it got here before we did
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

SRailNodeSA* CTrainTrackManagerSA::GetTrackNodes(std::uint8_t trackID) noexcept
{
    return trackID < MAX_TRACKS ? ms_pTrackNodes[trackID] : nullptr;
}

std::int32_t CTrainTrackManagerSA::GetTrackNodeCount(std::uint8_t trackID) noexcept
{
    return trackID < MAX_TRACKS ? ms_NumTrackNodes[trackID] : 0;
}

float CTrainTrackManagerSA::GetTrackLength(std::uint8_t trackID) const noexcept
{
    return trackID < MAX_TRACKS ? ms_TotalTrackLength[trackID] : 0.0f;
}

CTrainTrack* CTrainTrackManagerSA::CreateTrainTrack(const std::vector<CVector>& nodePositions, bool bLinkLastNodes)
{
    // Without the relocation a custom track ID would send the train code off the end of the built-in
    // arrays, so refuse to hand one out at all
    if (!ms_bArraysRelocated || nodePositions.size() < 2)
        return nullptr;

    for (std::size_t i = 0; i < m_Tracks.size(); i++)
    {
        if (m_Tracks[i] != nullptr)
            continue;

        std::uint8_t trackID = static_cast<std::uint8_t>(FIRST_CUSTOM_TRACK_ID + i);
        m_Tracks[i] = std::make_unique<CTrainTrackSA>(trackID, nodePositions, bLinkLastNodes);
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
