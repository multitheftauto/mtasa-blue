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
    // Track IDs below this are the game's own 4 built-in tracks (0-3); 255 means "no track"
    static constexpr std::uint8_t FIRST_CUSTOM_TRACK_ID = 4;

    virtual ~CTrainTrackManager() = default;

    // Returns nullptr if there are no free track ID slots left
    virtual CTrainTrack* CreateTrainTrack(const std::vector<CVector>& nodePositions, bool bLinkLastNodes) = 0;
    virtual void         DestroyTrainTrack(std::uint8_t trackID) = 0;
    virtual CTrainTrack* GetTrainTrack(std::uint8_t trackID) = 0;
};
