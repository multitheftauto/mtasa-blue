/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <vector>
#include <cstdint>

struct STrainTrackNodeSA
{
    CVector position;
    float   distanceFromStart;
};

class CTrainTrack
{
public:
    virtual ~CTrainTrack() = default;

    virtual std::uint8_t GetTrackID() const noexcept = 0;

    // Moves a node and recomputes the distances every node past it depends on
    virtual bool SetNodePosition(std::uint32_t nodeIndex, const CVector& position) noexcept = 0;
};
