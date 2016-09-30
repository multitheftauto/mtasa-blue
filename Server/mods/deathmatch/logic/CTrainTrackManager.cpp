/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CTrainTrackManager.cpp
*  PURPOSE:     Train Node Manager class
*  DEVELOPERS:  Cazomino05 <Cazomino05@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"
#include "CTrainTrack.h"
#include <TrackNodes.h>
#include <algorithm>

CTrainTrackManager::CTrainTrackManager ( )
{
    Reset();
}

CTrainTrack* CTrainTrackManager::CreateTrainTrack(const std::vector<STrackNode>& nodes, bool linkLastNodes, CElement* pParent, CXMLNode* pNode)
{
    if (m_Tracks.size() >= MaxTracks)
        return nullptr;

    // Create new train track
    auto pTrainTrack = new CTrainTrack(this, nodes, linkLastNodes, pParent, pNode);
    m_Tracks.push_back(pTrainTrack);

    return pTrainTrack;
}

void CTrainTrackManager::DestroyTrainTrack(CTrainTrack* pTrainTrack)
{
    m_Tracks.erase(std::remove(m_Tracks.begin(), m_Tracks.end(), pTrainTrack));
}

void CTrainTrackManager::Reset()
{
    // Clear tracks
    m_Tracks.clear();

    // Create default tracks
    for (std::size_t i = 0; i < 4; ++i)
    {
        // Create train tracks
        CreateTrainTrack(OriginalTrackNodes[i], true, nullptr, nullptr);
    }
}
