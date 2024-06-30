/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDummyPoolSA.cpp
 *  PURPOSE:     Dummy pool class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "StdInc.h"
#include "CDummyPoolSA.h"

CDummyPoolSA::CDummyPoolSA()
{
    m_ppDummyPoolInterface = (CPoolSAInterface<CEntitySAInterface>**)0xB744A0;
}

void CDummyPoolSA::RemoveAllBuildingLods()
{
    if (m_pLodBackup)
        return;

    m_pLodBackup = std::make_unique<std::array<CEntitySAInterface*, MAX_DUMMIES>>();

    for (int i = 0; i < MAX_DUMMIES; i++)
    {
        CEntitySAInterface* object = (*m_ppDummyPoolInterface)->GetObject(i);
        (*m_pLodBackup)[i] = object->m_pLod;
        object->m_pLod = nullptr;
    }
}

void CDummyPoolSA::RestoreAllBuildingsLods()
{
    if (!m_pLodBackup)
        return;

    for (int i = 0; i < MAX_DUMMIES; i++)
    {
        CEntitySAInterface* object = (*m_ppDummyPoolInterface)->GetObject(i);
        object->m_pLod = (*m_pLodBackup)[i];
    }

    m_pLodBackup.release();
}

void CDummyPoolSA::UpdateBuildingLods(void* oldPool, void* newPool)
{
    const uint32_t offset = (uint32_t)newPool - (uint32_t)oldPool;

    if (m_pLodBackup)
    {
        for (int i = 0; i < MAX_DUMMIES; i++)
        {
            if ((*m_pLodBackup)[i] != nullptr)
            {
                (*m_pLodBackup)[i] = (CEntitySAInterface*)((uint32_t)(*m_pLodBackup)[i] + offset);
            }
        }
    }
    else
    {
        for (int i = 0; i < MAX_DUMMIES; i++)
        {
            CEntitySAInterface* object = (*m_ppDummyPoolInterface)->GetObject(i);
            if (object->m_pLod)
            {
                object->m_pLod = (CEntitySAInterface*)((uint32_t)object->m_pLod + offset);
            }
        }
    }
}
