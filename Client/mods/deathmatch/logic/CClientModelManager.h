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

    CClientModel* FindModelByID(int iModelID);

    std::vector<CClientModel*> CClientModelManager::GetModelsByType(const eClientModelType type, const unsigned int minModelID = 0);


    void DeallocateModelsAllocatedByResource(CResource* pResource);

private:
    CClientModel** m_Models;
    unsigned int m_modelCount = 0;
};
