/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTrainTrackManager.cpp
 *  PURPOSE:     Train track entity manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientTrainTrack.h"
#include <game/CTrainTrackManager.h>

CClientTrainTrackManager::CClientTrainTrackManager(CClientManager* pManager)
{
    m_pManager = pManager;
}

CClientTrainTrackManager::~CClientTrainTrackManager()
{
    DeleteAll();
}

CClientTrainTrack* CClientTrainTrackManager::Get(ElementID ID)
{
    CClientEntity* pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTTRAINTRACK)
        return static_cast<CClientTrainTrack*>(pEntity);

    return nullptr;
}

CClientTrainTrack* CClientTrainTrackManager::GetByGameTrackID(std::uint8_t gameTrackID) const noexcept
{
    if (gameTrackID < CTrainTrackManager::FIRST_CUSTOM_TRACK_ID)
        return nullptr;

    for (CClientTrainTrack* pTrainTrack : m_TrainTracks)
    {
        if (pTrainTrack->GetGameTrackID() == gameTrackID)
            return pTrainTrack;
    }

    return nullptr;
}

void CClientTrainTrackManager::DeleteAll()
{
    std::list<CClientTrainTrack*> tracksToDelete = m_TrainTracks;
    for (CClientTrainTrack* pTrainTrack : tracksToDelete)
        delete pTrainTrack;

    m_TrainTracks.clear();
}
