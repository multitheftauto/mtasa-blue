/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainTrackManager.h"
#include "CTrainTrack.h"
#include <TrackNodes.h>
#include <algorithm>

CTrainTrackManager::CTrainTrackManager()
{
    // Create default tracks
    for (uchar i = 0; i < 4; ++i)
    {
        m_DefaultTracks[i] = CreateTrainTrack(OriginalTrackNodes[i], true, nullptr, i);
    }
}

CTrainTrack* CTrainTrackManager::CreateTrainTrack(const std::vector<STrackNode>& nodes, bool linkLastNodes, CElement* pParent, uchar defaultTrackId)
{
    if (m_Tracks.size() >= MaxTracks)
        return nullptr;

    // Create new train track
    auto pTrainTrack = new CTrainTrack(this, nodes, linkLastNodes, pParent, defaultTrackId);
    m_Tracks.push_back(pTrainTrack);

    return pTrainTrack;
}

void CTrainTrackManager::DestroyTrainTrack(CTrainTrack* pTrainTrack)
{
    m_Tracks.erase(std::remove(m_Tracks.begin(), m_Tracks.end(), pTrainTrack));
    if (pTrainTrack->IsDefault())
    {
        uchar defaultId = pTrainTrack->GetDefaultTrackId();
        m_DefaultTracks[defaultId] = nullptr;
    }
}

CTrainTrack* CTrainTrackManager::GetDefaultTrackByIndex(uchar index)
{
    if (index >= MaxDefaultTracks)
        return nullptr;
    return m_DefaultTracks[index];
}
