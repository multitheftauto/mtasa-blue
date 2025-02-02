/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPtrNodeSingleLinkPoolSA.h
 *  PURPOSE:     Custom implementation for the CPtrNodeSingleLinkPool pool
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
 
#include "CPoolSAInterface.h"
#include "CDynamicPool.h"
#include "CPtrNodeSingleListSA.h"
#include <game/CPtrNodeSingleLinkPool.h>

class CPtrNodeSingleLinkPoolSA final : public CPtrNodeSingleLinkPool
{
public:
    using pool_item_t = CPtrNodeSingleLink<void*>;
    using pool_t = CDynamicPool<pool_item_t, PoolGrowAddStrategy<MAX_POINTER_SINGLE_LINKS, MAX_POINTER_SINGLE_LINKS / 2>>;

    CPtrNodeSingleLinkPoolSA();

    std::size_t GetCapacity() const override { return m_customPool->GetCapacity(); }
    std::size_t GetUsedSize() const override { return m_customPool->GetUsedSize(); }

    bool Resize(std::size_t newSize) override { return m_customPool->SetCapacity(newSize); };
    void ResetCapacity() override { m_customPool->SetCapacity(MAX_POINTER_SINGLE_LINKS); };

    static auto* GetPoolInstance() { return m_customPool; } 
    static void StaticSetHooks();
private:
    static pool_t* m_customPool;
};
