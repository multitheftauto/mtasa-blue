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
#include "CClientModel.h"

#define MAX_MODEL_ID 20000

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

    void DeallocateModelsAllocatedByResource(CResource* pResource);

private:
    CClientModel* m_Models[MAX_MODEL_ID];
};
