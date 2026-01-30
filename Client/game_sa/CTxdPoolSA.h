/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CTxdPool.h>
#include "CPoolSAInterface.h"
#include "CBuildingSA.h"
#include "CTextureDictonarySA.h"

class CTxdPoolSA final : public CTxdPool
{
public:
    CTxdPoolSA();
    ~CTxdPoolSA() = default;

    std::uint32_t AllocateTextureDictonarySlot(std::uint32_t uiSlotID, std::string& strTxdName) override;
    void          RemoveTextureDictonarySlot(std::uint32_t uiTxdId) override;
    bool          IsFreeTextureDictonarySlot(std::uint32_t uiTxdId) override;

    std::uint32_t GetFreeTextureDictonarySlot() override;

    int GetPoolSize() const noexcept;
    int GetUsedSlotCount() const noexcept;

    // Access/modification helpers for slots we create
    CTextureDictonarySAInterface* GetTextureDictonarySlot(std::uint32_t uiTxdId) noexcept;
    bool                          SetTextureDictonarySlot(std::uint32_t uiTxdId, RwTexDictionary* pTxd, std::uint16_t usParentIndex) noexcept;

private:
    CPoolSAInterface<CTextureDictonarySAInterface>** m_ppTxdPoolInterface;
};
