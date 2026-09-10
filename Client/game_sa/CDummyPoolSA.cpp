/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDummyPoolSA.cpp
 *  PURPOSE:     Dummy pool class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "StdInc.h"
#include "CDummyPoolSA.h"
#include "CGameSA.h"
#include <game/CWorld.h>

extern CGameSA* pGame;

CDummyPoolSA::CDummyPoolSA()
{
    m_ppDummyPoolInterface = (CPoolSAInterface<CEntitySAInterface>**)0xB744A0;
}

void CDummyPoolSA::RemoveAllWithBackup()
{
    if (m_pOriginalElementsBackup)
        return;

    m_pOriginalElementsBackup = std::make_unique<pool_backup_t>();

    auto pDummyPool = (*m_ppDummyPoolInterface);
    for (auto i = 0; i < MAX_DUMMIES_DEFAULT; i++)
    {
        if (pDummyPool->IsContains(i))
        {
            CEntitySAInterface* building = pDummyPool->GetObject(i);

            pGame->GetWorld()->Remove(building, CDummyPool_Destructor);
            building->RemoveRWObjectWithReferencesCleanup();

            pDummyPool->Release(i);

            (*m_pOriginalElementsBackup)[i].first = true;
            std::memcpy((*m_pOriginalElementsBackup)[i].second, building, sizeof(CEntitySAInterface));
        }
        else
        {
            (*m_pOriginalElementsBackup)[i].first = false;
        }
    }
}

void CDummyPoolSA::RestoreBackup()
{
    if (!m_pOriginalElementsBackup)
        return;

    auto& originalData = *m_pOriginalElementsBackup;
    auto  pDummyPool = (*m_ppDummyPoolInterface);
    for (auto i = 0; i < MAX_DUMMIES_DEFAULT; i++)
    {
        if (originalData[i].first)
        {
            pDummyPool->AllocateAtNoInit(i);
            auto pDummy = pDummyPool->GetObject(i);
            std::memcpy(pDummy, &originalData[i].second, sizeof(CEntitySAInterface));

            pGame->GetWorld()->Add(pDummy, CDummyPool_Constructor);
        }
    }

    m_pOriginalElementsBackup = nullptr;
}

void CDummyPoolSA::UpdateBuildingLods(const std::uint32_t offset, const std::uintptr_t oldPoolStart, const std::uintptr_t oldPoolEnd)
{
    if (m_pOriginalElementsBackup)
        UpdateBackupLodOffset(offset, oldPoolStart, oldPoolEnd);
    else
        UpdateLodsOffestInPool(offset, oldPoolStart, oldPoolEnd);
}

// A dummy's LOD can point at another dummy just as easily as at a building, so only shift it
// when it actually falls inside the buildings pool range that just moved; otherwise the pointer
// is left as is, since it was never invalidated by this resize.
void CDummyPoolSA::UpdateBackupLodOffset(const std::uint32_t offset, const std::uintptr_t oldPoolStart, const std::uintptr_t oldPoolEnd)
{
    for (auto& it : *m_pOriginalElementsBackup)
    {
        if (it.first)
        {
            CEntitySAInterface* object = reinterpret_cast<CEntitySAInterface*>(&it.second);
            CEntitySAInterface* lod = object->GetLod();
            auto                lodAddress = reinterpret_cast<std::uintptr_t>(lod);
            if (lodAddress >= oldPoolStart && lodAddress < oldPoolEnd)
                object->SetLod(reinterpret_cast<CEntitySAInterface*>(lodAddress + offset));
        }
    }
}

void CDummyPoolSA::UpdateLodsOffestInPool(const std::uint32_t offset, const std::uintptr_t oldPoolStart, const std::uintptr_t oldPoolEnd)
{
    for (auto i = 0; i < (*m_ppDummyPoolInterface)->Size(); i++)
    {
        CEntitySAInterface* object = (*m_ppDummyPoolInterface)->GetObject(i);
        CEntitySAInterface* lod = object->GetLod();
        auto                lodAddress = reinterpret_cast<std::uintptr_t>(lod);
        if (lodAddress >= oldPoolStart && lodAddress < oldPoolEnd)
            object->SetLod(reinterpret_cast<CEntitySAInterface*>(lodAddress + offset));
    }
}
