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
#include <map>
#include <vector>
#include <memory>
#include <CVector.h>
#include "CClientModel.h"

#define MAX_MODEL_DFF_ID 20000
#define MAX_MODEL_TXD_ID 25000
#define MAX_MODEL_ID     25000

class CColModel;

// Identifies a (base model, scale) combination so identical scale requests can share one clone
struct SScaledColModelKey
{
    unsigned short usBaseModelID;
    int            iScaleX;  // Scale components quantized to 1/1000th to keep the cache key stable
    int            iScaleY;
    int            iScaleZ;

    bool operator<(const SScaledColModelKey& other) const
    {
        if (usBaseModelID != other.usBaseModelID)
            return usBaseModelID < other.usBaseModelID;
        if (iScaleX != other.iScaleX)
            return iScaleX < other.iScaleX;
        if (iScaleY != other.iScaleY)
            return iScaleY < other.iScaleY;
        return iScaleZ < other.iScaleZ;
    }
};

class CClientModelManager
{
    friend class CClientModel;

public:
    CClientModelManager();
    ~CClientModelManager(void);

    void RemoveAll(void);

    void Add(const std::shared_ptr<CClientModel>& pModel);
    bool Remove(const std::shared_ptr<CClientModel>& pModel);

    int GetFirstFreeModelID(void);
    int GetFreeTxdModelID();

    std::shared_ptr<CClientModel> FindModelByID(int iModelID);
    std::shared_ptr<CClientModel> Request(CClientManager* pManager, int iModelID, eClientModelType eType);

    std::vector<std::shared_ptr<CClientModel>> GetModelsByType(eClientModelType type, const unsigned int minModelID = 0);

    void DeallocateModelsAllocatedByResource(CResource* pResource);

    // Returns a model ID cloned from usBaseModelID whose collision is scaled by vecScale.
    // Identical (model, scale) requests share the same clone (refcounted). Returns -1 on
    // failure (no free model slot, or non-uniform scale with a collision that can't support it).
    // Each successful call must be paired with exactly one ReleaseScaledCollisionModel call.
    int AcquireScaledCollisionModel(unsigned short usBaseModelID, const CVector& vecScale);

    // Releases a reference acquired via AcquireScaledCollisionModel. Frees the clone once its
    // refcount reaches zero.
    void ReleaseScaledCollisionModel(int iClonedModelID);

private:
    struct SScaledColModelEntry
    {
        std::shared_ptr<CClientModel> pClonedModel;
        CColModel*                    pScaledColModel = nullptr;
        unsigned int                  uiRefCount = 0;
    };

    std::unique_ptr<std::shared_ptr<CClientModel>[]> m_Models;
    unsigned int                                     m_modelCount = 0;

    std::map<SScaledColModelKey, SScaledColModelEntry> m_ScaledColModels;
    std::map<int, SScaledColModelKey>                  m_ScaledColModelKeyByID;
};
