/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientModelManager.h
 *  PURPOSE:     Model manager class
 *
 *****************************************************************************/

class CClientModelManager;

#pragma once

#include <list>
#include <vector>
#include "CClientModel.h"

#define MAX_MODEL_DFF_ID 20000
#define MAX_MODEL_TXD_ID 25000
#define MAX_MODEL_ID     25000

class CClientModelManager
{
    friend class CClientModel;

public:
    CClientModelManager(class CClientManager* pManager);
    ~CClientModelManager(void);

    void RemoveAll(void);

    void Add(CClientModel* pModel);
    bool Remove(CClientModel* pModel);

    int GetFirstFreeModelID(void);
    int GetFreeTxdModelID();

    CClientModel* FindModelByID(int iModelID);
    CClientModel* Request(CClientManager* pManager, int iModelID, eClientModelType eType);

    std::vector<CClientModel*> CClientModelManager::GetModelsByType(const eClientModelType type, const unsigned int minModelID = 0);


    void DeallocateModelsAllocatedByResource(CResource* pResource);

private:
    CClientModel* m_Models[MAX_MODEL_ID];
    unsigned int m_modelCount = 0;
};
