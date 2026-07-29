/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainTrackManagerSA.h"

CTrainTrack* CTrainTrackManagerSA::CreateTrainTrack(const std::vector<CVector>& nodePositions, bool bLinkLastNodes)
{
    if (nodePositions.empty())
        return nullptr;

    for (std::size_t i = 0; i < m_Tracks.size(); i++)
    {
        if (m_Tracks[i] != nullptr)
            continue;

        std::uint8_t trackID = static_cast<std::uint8_t>(FIRST_CUSTOM_TRACK_ID + i);
        m_Tracks[i] = std::make_unique<CTrainTrackSA>(trackID, nodePositions, bLinkLastNodes);
        return m_Tracks[i].get();
    }

    return nullptr;
}

void CTrainTrackManagerSA::DestroyTrainTrack(std::uint8_t trackID)
{
    if (trackID < FIRST_CUSTOM_TRACK_ID)
        return;

    std::size_t index = static_cast<std::size_t>(trackID) - FIRST_CUSTOM_TRACK_ID;
    if (index < m_Tracks.size())
        m_Tracks[index].reset();
}

CTrainTrack* CTrainTrackManagerSA::GetTrainTrack(std::uint8_t trackID)
{
    if (trackID < FIRST_CUSTOM_TRACK_ID)
        return nullptr;

    std::size_t index = static_cast<std::size_t>(trackID) - FIRST_CUSTOM_TRACK_ID;
    if (index >= m_Tracks.size())
        return nullptr;

    return m_Tracks[index].get();
}
