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
#include <game/CTrainTrack.h>

struct STrackNode;
class CTrainTrackSA;

class CTrainTrackManagerSA : public CTrainTrackManager
{
public:
    CTrainTrackManagerSA();

    virtual CTrainTrack* CreateTrainTrack(const std::vector<STrackNode>& nodes, bool bLinkedLastNode) override;
    virtual void DestroyTrainTrack(CTrainTrack* pTrainTrack) override;

    virtual void Reset() override;

    void UpdateTrackData(CTrainTrackSA* pTrainTrack);

    virtual CTrainTrack* GetTrainTrackByIndex(uint trackIndex) override;

    virtual std::size_t GetNumberOfTrainTracks() const override { return m_Tracks.size(); }
    virtual const std::vector<std::unique_ptr<CTrainTrack>>& GetTrackNodes() const override { return m_Tracks; }

private:
    uint AllocateTrainTrackIndex();
    void PatchNumberOfTracks(std::uint8_t numTracks);

    std::vector<std::unique_ptr<CTrainTrack>> m_Tracks;

    // Arrays that are directly passed to SA
    uint m_CurrentTrackNodeSize = 0;
    STrackNode** m_TrackNodePointers = nullptr;
    float* m_TrackLengths = nullptr;
    std::uint32_t* m_NumberOfTrackNodes = nullptr;
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
- 0x6F6BD0: int GetTrainNodeNearPoint(float x, float y, float z, int* pTrackID) places track ID in *pTrackID and returns node ID
*/
