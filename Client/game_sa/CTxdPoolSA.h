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
    // Pool expansion limit: 5000 > 10000 > 20000 > 32768.
    // Capped at 32768: SA reads CBaseModelInfo::usTextureDictionary with
    // movsx, so indices >= 32768 become negative and crash SA functions.
    static constexpr int TXD_POOL_MAX_CAPACITY = 32768;

    CTxdPoolSA();
    ~CTxdPoolSA() = default;

    std::uint32_t AllocateTextureDictonarySlot(std::uint32_t uiSlotID, std::string& strTxdName) override;
    void          RemoveTextureDictonarySlot(std::uint32_t uiTxdId) override;
    bool          IsFreeTextureDictonarySlot(std::uint32_t uiTxdId) override;

    std::uint32_t GetFreeTextureDictonarySlot() override;
    std::uint32_t GetFreeTextureDictonarySlotInRange(std::uint32_t maxExclusive) override;
    std::uint32_t GetFreeTextureDictonarySlotAbove(std::uint32_t minInclusive) override;
    int           GetUsedSlotCountInRange(std::uint32_t maxExclusive) const override;

    int GetPoolSize() const noexcept;
    int GetUsedSlotCount() const noexcept;

    // Grows pool to newCapacity, preserving existing slots.
    // Returns false if newCapacity <= current or allocation fails.
    bool Resize(int newCapacity);

    // Access/modification helpers for slots we create
    CTextureDictonarySAInterface* GetTextureDictonarySlot(std::uint32_t uiTxdId) noexcept;
    bool                          SetTextureDictonarySlot(std::uint32_t uiTxdId, RwTexDictionary* pTxd, std::uint16_t usParentIndex) noexcept;

    // Streaming protection: prevents Hook_CTxdStore_RemoveRef from calling
    // CStreaming::RemoveModel when the ref count of a protected slot drops
    // to zero. This keeps SA from destroying the RwTexDictionary of MTA-
    // managed isolated TXD slots before MTA has orphaned their textures.
    void ProtectSlotFromStreaming(unsigned short usSlotId);
    void UnprotectSlotFromStreaming(unsigned short usSlotId);
    bool IsSlotProtectedFromStreaming(unsigned short usSlotId) const;

private:
    void InstallPoolHooks();

    CPoolSAInterface<CTextureDictonarySAInterface>** m_ppTxdPoolInterface;
};
