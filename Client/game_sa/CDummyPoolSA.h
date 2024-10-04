/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDummyPoolSA.h
 *  PURPOSE:     Dummy pool class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/CDummyPool.h>
#include "CEntitySA.h"
#include "CPoolSAInterface.h"
#include <memory>

class CDummyPoolSA : public CDummyPool
{
public:
    CDummyPoolSA();
    ~CDummyPoolSA() = default;

    void RemoveAllBuildingLods();
    void RestoreAllBuildingsLods();
    void UpdateBuildingLods(void* oldPool, void* newPool);

private:
    CPoolSAInterface<CEntitySAInterface>**  m_ppDummyPoolInterface;

    std::unique_ptr<std::array<CEntitySAInterface*, MAX_DUMMIES>> m_pLodBackup;
};
