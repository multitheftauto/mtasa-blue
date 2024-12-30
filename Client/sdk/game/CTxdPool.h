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
    virtual std::uint32_t AllocateTextureDictonarySlot(std::uint32_t uiSlotID, std::string& strTxdName) = 0;
    virtual void          RemoveTextureDictonarySlot(std::uint32_t uiTxdID) = 0;
    virtual bool          IsFreeTextureDictonarySlot(std::uint32_t uiTxdID) = 0;

    virtual std::uint16_t GetFreeTextureDictonarySlot() = 0;
};
