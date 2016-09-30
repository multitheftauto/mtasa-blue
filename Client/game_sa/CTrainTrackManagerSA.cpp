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

CTrainTrackManagerSA::CTrainTrackManagerSA()
{
    for (std::size_t i = 0; i < 4; ++i)
    {
        // Create train tracks
        auto pTrainTrack = CreateTrainTrack(OriginalTrackNodes[i], true);
    }

    // pGlobalTrainNodes 0xC38014
    MemPut<DWORD>(0x6F58D2, (DWORD)&m_TrackNodePointers); // 1
    MemPut<DWORD>(0x6F59FE, (DWORD)&m_TrackNodePointers); // 2
    MemPut<DWORD>(0x6F6C06, (DWORD)&m_TrackNodePointers); // 3
    MemPut<DWORD>(0x6F6D0A, (DWORD)&m_TrackNodePointers); // 4
    MemPut<DWORD>(0x6F6EB1, (DWORD)&m_TrackNodePointers); // 5
    MemPut<DWORD>(0x6F6F79, (DWORD)&m_TrackNodePointers); // 6
    MemPut<DWORD>(0x6F7442, (DWORD)&m_TrackNodePointers); // 7
    MemPut<DWORD>(0x6F7467, (DWORD)&m_TrackNodePointers); // 8
    MemPut<DWORD>(0x6F74EC, (DWORD)&m_TrackNodePointers); // 9
    MemPut<DWORD>(0x6F75B7, (DWORD)&m_TrackNodePointers); // 10
    MemPut<DWORD>(0x6F7B73, (DWORD)&m_TrackNodePointers); // 11
    MemPut<DWORD>(0x6F7C60, (DWORD)&m_TrackNodePointers); // 12
    MemPut<DWORD>(0x6F7DC5, (DWORD)&m_TrackNodePointers); // 13
    MemPut<DWORD>(0x6F7EE9, (DWORD)&m_TrackNodePointers); // 14
    MemPut<DWORD>(0x6F8007, (DWORD)&m_TrackNodePointers); // 15
    MemPut<DWORD>(0x6F809C, (DWORD)&m_TrackNodePointers); // 16
    MemPut<DWORD>(0x6F871C, (DWORD)&m_TrackNodePointers); // 17

    // fRailTrackOneLength 0xC37FEC
    MemPut<DWORD>(0x6F5BC4, (DWORD)&m_TrackLengths); // 1
    MemPut<DWORD>(0x6F5BD9, (DWORD)&m_TrackLengths); // 2
    MemPut<DWORD>(0x6F5C15, (DWORD)&m_TrackLengths); // 3
    MemPut<DWORD>(0x6F5C32, (DWORD)&m_TrackLengths); // 4
    MemPut<DWORD>(0x6F745D, (DWORD)&m_TrackLengths); // 5
    MemPut<DWORD>(0x6F8712, (DWORD)&m_TrackLengths); // 6
    MemPut<DWORD>(0x6F6FE8, (DWORD)&m_TrackLengths); // 7

    // iNumberOfTrackNodes 0xC38014
    MemPut<DWORD>(0x6F59EB, (DWORD)&m_NumberOfTrackNodes); // 1
    MemPut<DWORD>(0x6F6BF3, (DWORD)&m_NumberOfTrackNodes); // 2
    MemPut<DWORD>(0x6F6C96, (DWORD)&m_NumberOfTrackNodes); // 3
    MemPut<DWORD>(0x6F6CD1, (DWORD)&m_NumberOfTrackNodes); // 4
    MemPut<DWORD>(0x6F6D04, (DWORD)&m_NumberOfTrackNodes); // 5
    MemPut<DWORD>(0x6F6F52, (DWORD)&m_NumberOfTrackNodes); // 6
    MemPut<DWORD>(0x6F7DB6, (DWORD)&m_NumberOfTrackNodes); // 7
    MemPut<DWORD>(0x6F7F05, (DWORD)&m_NumberOfTrackNodes); // 8
    MemPut<DWORD>(0x6F7F12, (DWORD)&m_NumberOfTrackNodes); // 9
    MemPut<DWORD>(0x6F8001, (DWORD)&m_NumberOfTrackNodes); // 10
    MemPut<DWORD>(0x6F80B6, (DWORD)&m_NumberOfTrackNodes); // 11
    MemPut<DWORD>(0x6F80C3, (DWORD)&m_NumberOfTrackNodes); // 12
    MemPut<DWORD>(0x6F8723, (DWORD)&m_NumberOfTrackNodes); // 13
}

CTrainTrackSA* CTrainTrackManagerSA::CreateTrainTrack(const std::vector<STrackNode>& nodes, bool bLinkedLastNode)
{
    // Dynamically allocate new track
    auto index = AllocateTrainTrackIndex();
    auto pTrainTrack = std::make_unique<CTrainTrackSA>(index, false, this);

    // Update internal track data
    UpdateTrackData(pTrainTrack.get());

    // Add to tracks list
    m_Tracks.push_back(std::move(pTrainTrack));

    return m_Tracks.back().get();
}

void CTrainTrackManagerSA::UpdateTrackData(CTrainTrackSA* pTrainTrack)
{
    auto trackIndex = pTrainTrack->GetTrackID();

    // Update length
    m_TrackLengths[trackIndex] = pTrainTrack->GetLength();

    // Update track nodes amount
    m_NumberOfTrackNodes[trackIndex] = pTrainTrack->GetNumberOfNodes();

    // Update nodes pointers in array
    m_TrackNodePointers[trackIndex] = pTrainTrack->GetTrackNodes();
}

void CTrainTrackManagerSA::DestroyTrainTrack(CTrainTrackSA* pTrainTrack)
{
    // Remove track from arrays
    auto trackIndex = pTrainTrack->GetTrackID();
    m_TrackNodePointers[trackIndex] = nullptr;
    m_TrackLengths[trackIndex] = 0.0f;
    m_NumberOfTrackNodes[trackIndex] = 0;

    // Remove track from list
    m_Tracks.erase(std::remove(m_Tracks.begin(), m_Tracks.end(), pTrainTrack));
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
    std::memset(m_TrackNodePointers, 0, m_CurrentTrackNodeSize);
    auto trackLengths = new float[m_CurrentTrackNodeSize];
    auto numTrackNodes = new std::uint32_t[m_CurrentTrackNodeSize];
    
    // Copy previous data into these arrays
    std::memcpy(trackNodePtrs, m_TrackNodePointers, previousTrackNodeSize);
    std::memcpy(trackLengths, m_TrackLengths, previousTrackNodeSize);
    std::memcpy(numTrackNodes, m_NumberOfTrackNodes, previousTrackNodeSize);

    // Delete old memory
    delete m_TrackNodePointers;
    delete m_TrackLengths;
    delete m_NumberOfTrackNodes;

    // Replace with new
    m_TrackNodePointers = trackNodePtrs;
    m_TrackLengths = trackLengths;
    m_NumberOfTrackNodes = numTrackNodes;

    // Return new id
    return m_CurrentTrackNodeSize + 1;
}
