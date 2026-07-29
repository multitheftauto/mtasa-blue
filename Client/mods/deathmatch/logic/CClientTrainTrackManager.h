/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTrainTrackManager.h
 *  PURPOSE:     Train track entity manager class
 *
 *****************************************************************************/

#pragma once

#include "CClientCommon.h"
#include "CClientTrainTrack.h"
#include <list>

class CClientTrainTrackManager
{
    friend class CClientManager;
    friend class CClientTrainTrack;

public:
    unsigned int Count() const noexcept { return static_cast<unsigned int>(m_TrainTracks.size()); }
    static CClientTrainTrack* Get(ElementID ID);
    void                      DeleteAll();

private:
    CClientTrainTrackManager(class CClientManager* pManager);
    ~CClientTrainTrackManager();

    void AddToList(CClientTrainTrack* pTrainTrack) { m_TrainTracks.push_back(pTrainTrack); }
    void RemoveFromList(CClientTrainTrack* pTrainTrack) { m_TrainTracks.remove(pTrainTrack); }

    class CClientManager*          m_pManager;
    std::list<CClientTrainTrack*> m_TrainTracks;
};
