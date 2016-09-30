/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTrainTrackManagerSA.h
*  PURPOSE:     Header file for train node manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include <game/CTrainTrackManager.h>

struct STrackNode;
class CTrainTrackSA;

#define COMP_NumberOfTracks                     0x6F6CA9

class CTrainTrackManagerSA : public CTrainTrackManager
{
public:
    CTrainTrackManagerSA();

    CTrainTrackSA* CreateTrainTrack(const std::vector<STrackNode>& nodes, bool bLinkedLastNode);
    void DestroyTrainTrack(CTrainTrackSA* pTrainTrack);

    void UpdateTrackData(CTrainTrackSA* pTrainTrack);

    inline CTrainTrackSA* GetTrainTrack(uint trackId) { return m_Tracks[trackId].get(); }

private:
    uint AllocateTrainTrackIndex();

    std::vector<std::unique_ptr<CTrainTrackSA>> m_Tracks;

    // Arrays that are directly passed to SA
    uint m_CurrentTrackNodeSize;
    STrackNode** m_TrackNodePointers;
    float* m_TrackLengths;
    std::uint32_t* m_NumberOfTrackNodes;
};

/*
DOCUMENTATION:
- m_Tracks: Currently used train tracks (includes new tracks as well)
- m_TrackNodePointers: Contains ALL track nodes for ALL tracks
- m_TrackLengths: Contains overall track lengths

MEMORY ADDRESSES:
- 0xC38014: NUM_RAILTRACKS dwords
- 0xC37FEC: Track Length floats
- 0xC38024: NUM_RAILTRACKS pointers to arrays of STrackNode
*/
