/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CTxdPool
{
public:
    // Maximum TXD slot index whose streaming ID (index + 20000) fits within
    // SA's ms_aInfoForModel[26316].  Isolation TXDs prefer [0, 6316).
    // Script TXDs should prefer slots above this to avoid contention.
    static constexpr int MAX_STREAMING_TXD_SLOT = 6316;

    virtual std::uint32_t AllocateTextureDictonarySlot(std::uint32_t uiSlotID, std::string& strTxdName) = 0;
    virtual void          RemoveTextureDictonarySlot(std::uint32_t uiTxdID) = 0;
    virtual bool          IsFreeTextureDictonarySlot(std::uint32_t uiTxdID) = 0;

    virtual std::uint32_t GetFreeTextureDictonarySlot() = 0;

    // Returns the first free slot in [0, maxExclusive).
    virtual std::uint32_t GetFreeTextureDictonarySlotInRange(std::uint32_t maxExclusive) = 0;

    // Returns the first free slot in [minInclusive, poolSize), or -1 if none.
    virtual std::uint32_t GetFreeTextureDictonarySlotAbove(std::uint32_t minInclusive) = 0;

    // Returns the number of occupied slots in [0, maxExclusive).
    virtual int GetUsedSlotCountInRange(std::uint32_t maxExclusive) const = 0;
};
