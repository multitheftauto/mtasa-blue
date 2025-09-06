/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CTrainTrack.h"

class CTrainTrackManager
{
public:
    CTrainTrackManager();
    void Reset();

    CTrainTrack* CreateTrainTrack(const std::vector<STrackNode>& nodes, bool linkLastNodes, CElement* pParent, uchar defaultTrackId = 0xFF);
    void         DestroyTrainTrack(CTrainTrack* pTrainTrack);

    const std::vector<CTrainTrack*>& GetTracks() { return m_Tracks; }

    CTrainTrack* GetTrainTrackByIndex(unsigned int index);

private:
    constexpr static std::size_t MaxTracks = 255;
    std::vector<CTrainTrack*>    m_Tracks;
};
