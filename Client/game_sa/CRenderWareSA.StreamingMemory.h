/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CRenderWareSA.StreamingMemory.h
 *  PURPOSE:     Streaming memory purging (MakeSpaceFor) to reduce OOM
 *
 *****************************************************************************/

#pragma once

#include <cstdint>

namespace StreamingMemory
{
    void PrepareStreamingMemoryForSize(std::uint32_t estimatedBytes);
}
