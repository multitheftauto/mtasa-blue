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
#include "CClientTrainTrackManager.h"

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
