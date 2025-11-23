/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CIplStore.cpp
 *  PURPOSE:     IPL store class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include "CIplStoreSA.h"
#include "CQuadTreeNodeSA.h"

static auto gIplQuadTree = (CQuadTreeNodesSAInterface<CIplSAInterface>**)0x8E3FAC;

CIplStoreSA::CIplStoreSA() : m_isStreamingEnabled(true), m_ppIplPoolInterface((CPoolSAInterface<CIplSAInterface>**)0x8E3FB0)
{
}

void CIplStoreSA::UnloadAndDisableStreaming(int iplId)
{
    // Is pool valid?
    if (!m_ppIplPoolInterface)
        return;

    // Is pool object valid?
    auto pool = *m_ppIplPoolInterface;
    if (!pool)
        return;

    // Is IPL in pool?
    if (!pool->IsContains(iplId))
        return;

    // Is IPL object valid?
    auto ipl = pool->GetObject(iplId);
    if (!ipl)
        return;

    typedef void*(__cdecl * Function_EnableStreaming)(int);
    ((Function_EnableStreaming)(0x405890))(iplId);
}

void CIplStoreSA::EnableStreaming(int iplId)
{
    // Is pool valid?
    if (!m_ppIplPoolInterface)
        return;

    // Is pool object valid?
    auto pool = *m_ppIplPoolInterface;
    if (!pool)
        return;

    // Is IPL in pool?
    if (!pool->IsContains(iplId))
        return;

    // Is IPL object valid?
    auto ipl = pool->GetObject(iplId);
    if (!ipl)
        return;

    ipl->bDisabledStreaming = false;

    if (*gIplQuadTree)
        (*gIplQuadTree)->AddItem(ipl, &ipl->rect);
}

void CIplStoreSA::SetDynamicIplStreamingEnabled(bool state)
{
    if (m_isStreamingEnabled == state)
        return;

    // Ipl with 0 index is generic
    // We don't unload this IPL

    auto pool = *m_ppIplPoolInterface;
    if (!pool)
        return;

    // Collect all IPL ids
    std::vector<int> iplIds;
    iplIds.reserve(pool->m_nSize);

    for (int i = 1; i < pool->m_nSize; i++)
    {
        if (pool->IsContains(i))
            iplIds.push_back(i);
    }

    // Now enable/disable streaming for all IPLs
    if (!state)
    {
        for (int iplId : iplIds)
            UnloadAndDisableStreaming(iplId);

        if (*gIplQuadTree)
            (*gIplQuadTree)->RemoveAllItems();
    }
    else
    {
        for (int iplId : iplIds)
            EnableStreaming(iplId);
    }

    m_isStreamingEnabled = state;
}

void CIplStoreSA::SetDynamicIplStreamingEnabled(bool state, std::function<bool(CIplSAInterface* ipl)> filter)
{
    if (m_isStreamingEnabled == state)
        return;

    // Ipl with 0 index is generic
    // We don't unload this IPL

    auto pool = *m_ppIplPoolInterface;
    if (!pool)
        return;

    // Collect IPL ids that match the filter
    std::vector<int> iplIds;
    iplIds.reserve(pool->m_nSize);

    for (int i = 1; i < pool->m_nSize; i++)
    {
        auto ipl = pool->GetObject(i);
        if (ipl && pool->IsContains(i) && filter(ipl))
            iplIds.push_back(i);
    }

    // Apply the streaming state change
    if (!state)
    {
        for (int iplId : iplIds)
            UnloadAndDisableStreaming(iplId);

        if (*gIplQuadTree)
            (*gIplQuadTree)->RemoveAllItems();
    }
    else
    {
        for (int iplId : iplIds)
            EnableStreaming(iplId);
    }

    m_isStreamingEnabled = state;
}
