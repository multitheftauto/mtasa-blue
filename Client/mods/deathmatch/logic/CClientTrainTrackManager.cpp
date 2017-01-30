/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientTrainTrackManager.h
*  PURPOSE:     Train track manager header
*
*****************************************************************************/
#include <StdInc.h>
#include <TrackNodes.h>
#include <game/CTrainTrackManager.h>

void CClientTrainTrackManager::Reset()
{
    // Reset game
    g_pGame->GetTrainTrackManager()->Reset();

    // Create default tracks
    for (std::size_t i = 0; i < 4; ++i)
    {
        // Delete old default tracks if exist
        CClientTrainTrack* pTrainTrack = GetTrainTrackByIndex(i);
        if (pTrainTrack)
            delete pTrainTrack;

        // Create train tracks
        pTrainTrack = new CClientTrainTrack(INVALID_ELEMENT_ID, OriginalTrackNodes[i], true);
        pTrainTrack->MakeSystemEntity();
    }
}

CClientTrainTrack* CClientTrainTrackManager::Get(ElementID ID)
{
    auto pEntity = CElementIDs::GetElement(ID);
    if (pEntity && pEntity->GetType() == CCLIENTTRAINTRACK)
    {
        return static_cast<CClientTrainTrack*>(pEntity);
    }

    return nullptr;
}

CClientTrainTrack* CClientTrainTrackManager::Get(CTrainTrack* pTrainTrack)
{
    auto& iter = m_Map.find(pTrainTrack);
    if (iter != m_Map.end())
        return iter->second;

    return nullptr;
}

CClientTrainTrack* CClientTrainTrackManager::GetTrainTrackByIndex(unsigned int index)
{
    return Get(g_pGame->GetTrainTrackManager()->GetTrainTrackByIndex(index));
}

void CClientTrainTrackManager::RegisterTrainTrack(CClientTrainTrack* pTrainTrack)
{
    m_Map.insert({ pTrainTrack->GetTrainTrack(), pTrainTrack });
}

void CClientTrainTrackManager::UnregisterTrainTrack(CClientTrainTrack* pTrainTrack)
{
    m_Map.erase(pTrainTrack->GetTrainTrack());
}
