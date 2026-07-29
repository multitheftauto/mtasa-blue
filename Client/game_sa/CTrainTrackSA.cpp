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

    float fDistance = 0.0f;
    for (std::size_t i = 0; i < nodePositions.size(); i++)
    {
        if (i > 0)
            fDistance += Distance2D(nodePositions[i], nodePositions[i - 1]);

        m_Nodes.push_back({nodePositions[i], fDistance});
    }

    m_fTotalLength = fDistance;
    if (bLinkLastNodes && nodePositions.size() > 1)
        m_fTotalLength += Distance2D(nodePositions.back(), nodePositions.front());
}
