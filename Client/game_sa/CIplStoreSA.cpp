/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CIplStore.cpp
 *  PURPOSE:     IPL store class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
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
    typedef void*(__cdecl * Function_EnableStreaming)(int);
    ((Function_EnableStreaming)(0x405890))(iplId);
}

void CIplStoreSA::EnableStreaming(int iplId)
{
    auto ipl = (*m_ppIplPoolInterface)->GetObject(iplId);
    ipl->bDisabledStreaming = false;

    (*gIplQuadTree)->AddItem(ipl, &ipl->rect);
}

void CIplStoreSA::SetDynamicIplStreamingEnabled(bool state)
{
    if (m_isStreamingEnabled == state)
        return;

    // Ipl with 0 index is generic
    // We don't unload this IPL

    auto pPool = *m_ppIplPoolInterface;
    if (!state)
    {
        for (int i = 1; i < pPool->m_nSize; i++)
        {
            if (pPool->IsContains(i))
            {
                UnloadAndDisableStreaming(i);
            }
        }
        (*gIplQuadTree)->RemoveAllItems();
    }
    else
    {
        for (int i = 1; i < pPool->m_nSize; i++)
        {
            if (pPool->IsContains(i))
            {
                EnableStreaming(i);
            }
        }
    }

    m_isStreamingEnabled = state;
}

void CIplStoreSA::SetDynamicIplStreamingEnabled(bool state, std::function<bool(CIplSAInterface* ipl)> filter)
{
    if (m_isStreamingEnabled == state)
        return;

    // Ipl with 0 index is generic
    // We don't unload this IPL

    auto pPool = *m_ppIplPoolInterface;
    if (!state)
    {
        for (int i = 1; i < pPool->m_nSize; i++)
        {
            if (pPool->IsContains(i) && filter(pPool->GetObject(i)))
            {
                UnloadAndDisableStreaming(i);
            }
        }
        (*gIplQuadTree)->RemoveAllItems();
    }
    else
    {
        for (int i = 1; i < pPool->m_nSize; i++)
        {
            if (pPool->IsContains(i) && filter(pPool->GetObject(i)))
            {
                EnableStreaming(i);
            }
        }
    }

    m_isStreamingEnabled = state;
}
