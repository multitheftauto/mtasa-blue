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

namespace
{
    // The game only cares about horizontal distance along the track, not height changes
    float Distance2D(const CVector& a, const CVector& b)
    {
        float fDeltaX = a.fX - b.fX;
        float fDeltaY = a.fY - b.fY;
        return std::sqrt(fDeltaX * fDeltaX + fDeltaY * fDeltaY);
    }
}  // namespace

CTrainTrackSA::CTrainTrackSA(std::uint8_t trackID, const std::vector<CVector>& nodePositions, bool bLinkLastNodes)
    : m_TrackID(trackID), m_fTotalLength(0.0f), m_bLinkLastNodes(bLinkLastNodes)
{
    m_Nodes.reserve(nodePositions.size());
    for (const auto& position : nodePositions)
        m_Nodes.push_back({position, 0.0f});

    RecomputeDistances();
}

bool CTrainTrackSA::SetNodePosition(std::uint32_t nodeIndex, const CVector& position) noexcept
{
    if (nodeIndex >= m_Nodes.size())
        return false;

    m_Nodes[nodeIndex].position = position;
    RecomputeDistances();
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

    m_fTotalLength = fDistance;
    if (m_bLinkLastNodes && m_Nodes.size() > 1)
        m_fTotalLength += Distance2D(m_Nodes.back().position, m_Nodes.front().position);
}
