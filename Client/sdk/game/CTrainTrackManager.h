/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CTrackTrackManager.h
*  PURPOSE:     Train Track Manager Interface
*  DEVELOPERS:  Cazomino05 < Cazomino05@gmail.com >
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once
#include <vector>
#include <memory>
#include "CTrainTrack.h"

class CTrainTrackManager
{
public:
    virtual CTrainTrack* CreateTrainTrack(const std::vector<STrackNode>& nodes, bool bLinkedLastNode) = 0;
    virtual void DestroyTrainTrack(CTrainTrack* pTrainTrack) = 0;

    virtual CTrainTrack* GetTrainTrackByIndex(unsigned int trackIndex) = 0;

    virtual std::size_t GetNumberOfTrainTracks() const = 0;
    virtual const std::vector<std::unique_ptr<CTrainTrack>>& GetTrackNodes() const = 0;
};
