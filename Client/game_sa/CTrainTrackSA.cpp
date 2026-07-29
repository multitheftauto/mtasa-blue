/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CTrainTrackSA.h"
#include "CTrainTrackManagerSA.h"
#include <algorithm>
#include <cmath>

namespace
{
    // The game only cares about horizontal distance along the track, not height changes
    float Distance2D(const CVector& a, const CVector& b)
    {
        float fDeltaX = a.fX - b.fX;
        float fDeltaY = a.fY - b.fY;
        return std::sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY);
    }

    // Node coordinates are stored as a short of eighths of a unit, distances as a word of thirds
    constexpr float POSITION_SCALE = 8.0f;
    constexpr float DISTANCE_SCALE = 3.0f;

    short QuantisePosition(float fValue)
    {
        const float fScaled = std::round(fValue * POSITION_SCALE);
        return static_cast<short>(std::clamp(fScaled, -32768.0f, 32767.0f));
    }

    WORD QuantiseDistance(float fValue)
    {
        const float fScaled = std::round(fValue * DISTANCE_SCALE);
        return static_cast<WORD>(std::clamp(fScaled, 0.0f, 65535.0f));
    }
}  // namespace

CTrainTrackSA::CTrainTrackSA(std::uint8_t trackID, const std::vector<CVector>& nodePositions) : m_TrackID(trackID), m_fTotalLength(0.0f)
{
    m_Nodes.reserve(nodePositions.size());
    for (const auto& position : nodePositions)
        m_Nodes.push_back({position, 0.0f});

    m_GameNodes.resize(m_Nodes.size());

    RecomputeDistances();
    PublishToGame();
}

CTrainTrackSA::~CTrainTrackSA()
{
    // Make sure no train is left walking a node array that's about to be freed
    CTrainTrackManagerSA::SetTrackData(m_TrackID, nullptr, 0, 0.0f);
}

bool CTrainTrackSA::SetNodePosition(std::uint32_t nodeIndex, const CVector& position) noexcept
{
    if (nodeIndex >= m_Nodes.size())
        return false;

    m_Nodes[nodeIndex].position = position;
    RecomputeDistances();
    PublishToGame();
    return true;
}

void CTrainTrackSA::RecomputeDistances() noexcept
{
    float fDistance = 0.0f;
    for (std::size_t i = 0; i < m_Nodes.size(); i++)
    {
        if (i > 0)
            fDistance += Distance2D(m_Nodes[i].position, m_Nodes[i - 1].position);

        m_Nodes[i].distanceFromStart = fDistance;
    }

    // The train code wraps at the total length, so every track is a loop: the run from the last node
    // back to the first counts towards the length
    if (m_Nodes.size() > 1)
        fDistance += Distance2D(m_Nodes.back().position, m_Nodes.front().position);

    m_fTotalLength = fDistance;
}

void CTrainTrackSA::PublishToGame() noexcept
{
    // Both vectors are sized once in the constructor and the node count never changes after that
    for (std::size_t i = 0; i < m_Nodes.size(); i++)
    {
        SRailNodeSA& gameNode = m_GameNodes[i];
        gameNode.sX = QuantisePosition(m_Nodes[i].position.fX);
        gameNode.sY = QuantisePosition(m_Nodes[i].position.fY);
        gameNode.sZ = QuantisePosition(m_Nodes[i].position.fZ);
        gameNode.sRailDistance = QuantiseDistance(m_Nodes[i].distanceFromStart);
        gameNode.padding = 0;
    }

    CTrainTrackManagerSA::SetTrackData(m_TrackID, m_GameNodes.data(), static_cast<std::int32_t>(m_GameNodes.size()), m_fTotalLength);
}
