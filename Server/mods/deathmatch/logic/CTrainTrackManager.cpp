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
    Reset();
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
}

CTrainTrack* CTrainTrackManager::GetTrainTrackByIndex(unsigned int index)
{
    if (index >= m_Tracks.size())
        return nullptr;

    return m_Tracks[index];
}

void CTrainTrackManager::Reset()
{
    // Clear tracks
    m_Tracks.clear();

    // Create default tracks
    for (std::size_t i = 0; i < 4; ++i)
    {
        // Create train tracks
        CreateTrainTrack(OriginalTrackNodes[i], true, nullptr, i);
    }
}
