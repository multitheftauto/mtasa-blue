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

    CTrainTrack* CreateTrainTrack(const std::vector<STrackNode>& nodes, bool linkLastNodes, CElement* pParent, uchar defaultTrackId = 0xFF);
    void         DestroyTrainTrack(CTrainTrack* pTrainTrack);

    const std::vector<CTrainTrack*>& GetTracks() { return m_Tracks; }

    CTrainTrack* GetDefaultTrackByIndex(uchar index);

private:
    constexpr static std::size_t MaxTracks = 255;
    constexpr static std::size_t MaxDefaultTracks = 4;

    std::vector<CTrainTrack*> m_Tracks;

    CTrainTrack* m_DefaultTracks[MaxDefaultTracks];
};
