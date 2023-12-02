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
#include <memory>
#include "CClientModel.h"

#define MAX_MODEL_DFF_ID 20000
#define MAX_MODEL_TXD_ID 25000
#define MAX_MODEL_ID     25000

struct SModelFrameInfo
{
    CMatrix matrix;
};

class CClientModelManager
{
    friend class CClientModel;

public:
    CClientModelManager::CClientModelManager();
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

    void SetModelFramePosition(uint16_t modelId, std::string& frameName, CVector vecPosition);
    void SetModelFrameRotation(uint16_t modelId, std::string& frameName, CVector vecRotation);
    void SetModelFrameScale(uint16_t modelId, std::string& frameName, CVector vecScale);
    bool GetModelFrameMatrix(uint16_t modelId, std::string& frameName, CMatrix& matrix);
    bool GetModelFramePosition(uint16_t modelId, std::string& frameName, CVector& vecPosition);
    bool GetModelFrameRotation(uint16_t modelId, std::string& frameName, CVector& vecRotation);
    bool GetModelFrameScale(uint16_t modelId, std::string& frameName, CVector& vecScale);
    bool ResetModelFrame(uint16_t modelId, std::string& frameName);
    bool ResetModelFrame(uint16_t modelId);
    bool TryGetModelFrameInfos(uint16_t modelId, std::unordered_map<std::string, SModelFrameInfo>& infos);

private:
    void InitializeModelFrameInfo(uint16_t modelId, std::string& frameName);
    std::unique_ptr<std::shared_ptr<CClientModel>[]>                               m_Models;
    std::unordered_map<uint16_t, std::unordered_map<std::string, SModelFrameInfo>> m_modelFramesInfo;
    unsigned int                                                                   m_modelCount = 0;
};
