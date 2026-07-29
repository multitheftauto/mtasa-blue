/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientTrainTrackManager.cpp
 *  PURPOSE:     Train track entity manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientTrainTrack.h"

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

void CClientTrainTrackManager::DeleteAll()
{
    std::list<CClientTrainTrack*> tracksToDelete = m_TrainTracks;
    for (CClientTrainTrack* pTrainTrack : tracksToDelete)
        delete pTrainTrack;

    m_TrainTracks.clear();
}
