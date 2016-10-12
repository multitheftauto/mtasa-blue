/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrackManagerSA.cpp
*  PURPOSE:     Train Node Manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include <TrackNodes.h>
#include <algorithm>
#include "CTrainTrackSA.h"

CTrainTrackManagerSA::CTrainTrackManagerSA()
{
    Reset();

    // Disable CTrain::InitTrains
    MemPut(0x6F7440, 0xC3); // Write retn
}

CTrainTrack* CTrainTrackManagerSA::CreateTrainTrack(const std::vector<STrackNode>& nodes, bool bLinkedLastNode)
{
    if (m_Tracks.size() >= 255)
        return nullptr;

    // Dynamically allocate new track
    auto index = AllocateTrainTrackIndex();
    auto pTrainTrack = std::make_unique<CTrainTrackSA>(index, nodes, bLinkedLastNode, this);

    // Add to tracks list
    m_Tracks.push_back(std::move(pTrainTrack));

    // Patch track count
    PatchNumberOfTracks(static_cast<std::uint8_t>(m_Tracks.size()));

    return m_Tracks.back().get();
}

void CTrainTrackManagerSA::UpdateTrackData(CTrainTrackSA* pTrainTrack)
{
    auto trackIndex = pTrainTrack->GetIndex();

    // Update length
    m_TrackLengths[trackIndex] = pTrainTrack->GetLength();

    // Update track nodes amount
    m_NumberOfTrackNodes[trackIndex] = pTrainTrack->GetNumberOfNodes();

    // Update nodes pointers in array
    m_TrackNodePointers[trackIndex] = pTrainTrack->GetNodesData();
}

CTrainTrack* CTrainTrackManagerSA::GetTrainTrackByIndex(uint trackIndex)
{
    auto iter = std::find_if(m_Tracks.begin(), m_Tracks.end(), [trackIndex](auto& pTrainTrack) { return pTrainTrack->GetIndex() == trackIndex; });
    if (iter == m_Tracks.end())
        return nullptr;

    return iter->get();
}

void CTrainTrackManagerSA::DestroyTrainTrack(CTrainTrack* pTrainTrack)
{
    // Remove track from arrays
    auto trackIndex = pTrainTrack->GetIndex();
    m_TrackNodePointers[trackIndex] = nullptr; // Mark as free
    m_TrackLengths[trackIndex] = 0.0f;
    m_NumberOfTrackNodes[trackIndex] = 0;

    // Remove track from list
    m_Tracks.erase(std::remove_if(m_Tracks.begin(), m_Tracks.end(), [pTrainTrack](auto& pTrack) { return pTrack.get() == pTrainTrack; }));

    // Patch track count
    PatchNumberOfTracks(static_cast<std::uint8_t>(m_Tracks.size()));
}

void CTrainTrackManagerSA::Reset()
{
    // Clear tracks
    m_Tracks.clear();

    // Clear SA data
    if (m_TrackNodePointers != nullptr)
    {
        delete m_TrackNodePointers;
        delete m_TrackLengths;
        delete m_NumberOfTrackNodes;
    }
    m_TrackNodePointers = nullptr;
    m_TrackLengths = nullptr;
    m_NumberOfTrackNodes = nullptr;
    m_CurrentTrackNodeSize = 0;

    // Create default tracks
    for (std::size_t i = 0; i < 4; ++i)
    {
        // Create train tracks
        CreateTrainTrack(OriginalTrackNodes[i], true);
    }
}

uint CTrainTrackManagerSA::AllocateTrainTrackIndex()
{
    // Check if there is a free index
    for (uint i = 0; i < m_CurrentTrackNodeSize; ++i)
    {
        if (m_TrackNodePointers[i] == nullptr)
            return i;
    }

    // We ran out-of-space, so allocate some more tracks
    auto previousTrackNodeSize = m_CurrentTrackNodeSize;
    m_CurrentTrackNodeSize += 10;
    auto trackNodePtrs = new STrackNode*[m_CurrentTrackNodeSize];
    std::memset(trackNodePtrs, 0, m_CurrentTrackNodeSize * sizeof(STrackNode**));
    auto trackLengths = new float[m_CurrentTrackNodeSize];
    auto numTrackNodes = new std::uint32_t[m_CurrentTrackNodeSize];
    
    if (m_TrackNodePointers != nullptr)
    {
        // Copy previous data into these arrays
        std::memcpy(trackNodePtrs, m_TrackNodePointers, previousTrackNodeSize * sizeof(STrackNode**));
        std::memcpy(trackLengths, m_TrackLengths, previousTrackNodeSize * sizeof(float*));
        std::memcpy(numTrackNodes, m_NumberOfTrackNodes, previousTrackNodeSize * sizeof(std::uint32_t*));

        // Delete old memory
        delete m_TrackNodePointers;
        delete m_TrackLengths;
        delete m_NumberOfTrackNodes;
    }

    // Replace with new
    m_TrackNodePointers = trackNodePtrs;
    m_TrackLengths = trackLengths;
    m_NumberOfTrackNodes = numTrackNodes;

    // Patch references
    PatchReferences();

    // Return new id
    return previousTrackNodeSize;
}

void CTrainTrackManagerSA::PatchReferences()
{
    // pGlobalTrainNodes 0xC38014
    MemPut(0x6F58D2, m_TrackNodePointers); // 1
    MemPut(0x6F59FE, m_TrackNodePointers); // 2
    MemPut(0x6F6C06, m_TrackNodePointers); // 3
    MemPut(0x6F6D0A, m_TrackNodePointers); // 4
    MemPut(0x6F6EB1, m_TrackNodePointers); // 5
    MemPut(0x6F6F79, m_TrackNodePointers); // 6
    MemPut(0x6F7442, m_TrackNodePointers); // 7
    MemPut(0x6F7467, m_TrackNodePointers); // 8
    MemPut(0x6F74EC, m_TrackNodePointers); // 9
    MemPut(0x6F75B7, m_TrackNodePointers); // 10
    MemPut(0x6F7B73, m_TrackNodePointers); // 11
    MemPut(0x6F7C60, m_TrackNodePointers); // 12
    MemPut(0x6F7DC5, m_TrackNodePointers); // 13
    MemPut(0x6F7EE9, m_TrackNodePointers); // 14
    MemPut(0x6F8007, m_TrackNodePointers); // 15
    MemPut(0x6F809C, m_TrackNodePointers); // 16
    MemPut(0x6F871C, m_TrackNodePointers); // 17

    // fRailTrackOneLength 0xC37FEC
    MemPut(0x6F5BC4, m_TrackLengths); // 1
    MemPut(0x6F5BD9, m_TrackLengths); // 2
    MemPut(0x6F5C15, m_TrackLengths); // 3
    MemPut(0x6F5C32, m_TrackLengths); // 4
    MemPut(0x6F745D, m_TrackLengths); // 5
    MemPut(0x6F8712, m_TrackLengths); // 6
    MemPut(0x6F6FE8, m_TrackLengths); // 7

    // iNumberOfTrackNodes 0xC38014
    MemPut(0x6F59EB, m_NumberOfTrackNodes); // 1
    MemPut(0x6F6BF3, m_NumberOfTrackNodes); // 2
    MemPut(0x6F6C96, m_NumberOfTrackNodes); // 3
    MemPut(0x6F6CD1, m_NumberOfTrackNodes); // 4
    MemPut(0x6F6D04, m_NumberOfTrackNodes); // 5
    MemPut(0x6F6F52, m_NumberOfTrackNodes); // 6
    MemPut(0x6F7DB6, m_NumberOfTrackNodes); // 7
    MemPut(0x6F7F05, m_NumberOfTrackNodes); // 8
    MemPut(0x6F7F12, m_NumberOfTrackNodes); // 9
    MemPut(0x6F8001, m_NumberOfTrackNodes); // 10
    MemPut(0x6F80B6, m_NumberOfTrackNodes); // 11
    MemPut(0x6F80C3, m_NumberOfTrackNodes); // 12
    MemPut(0x6F8723, m_NumberOfTrackNodes); // 13
}

void CTrainTrackManagerSA::PatchNumberOfTracks(std::uint8_t numTracks)
{
    // .text:006F6CA7 cmp esi, 4
    MemPut(0x6F6CA9, numTracks);
}
