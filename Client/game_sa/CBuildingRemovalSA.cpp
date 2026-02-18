/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingRemovalSA.cpp
 *  PURPOSE:     Building removal logic
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CBuildingRemovalSA.h"

#include "CWorldSA.h"
#include "CGameSA.h"
#include "CPoolsSA.h"
#include "HookSystem.h"

extern CGameSA* pGame;

// Checks whether pEntity still occupies a live slot in the building, dummy, or object pool.
static bool IsEntityPoolSlotOccupied(CEntitySAInterface* pEntity)
{
    CPoolSAInterface<CBuildingSAInterface>* pBuildingPool = *reinterpret_cast<CPoolSAInterface<CBuildingSAInterface>**>(CLASS_CBuildingPool);
    if (pBuildingPool)
    {
        std::int32_t iIndex = pBuildingPool->GetObjectIndexSafe(reinterpret_cast<CBuildingSAInterface*>(pEntity));
        if (iIndex != -1)
            return pBuildingPool->IsContains(iIndex);
    }

    CPoolSAInterface<CEntitySAInterface>* pDummyPool = *reinterpret_cast<CPoolSAInterface<CEntitySAInterface>**>(CLASS_CDummyPool);
    if (pDummyPool)
    {
        std::int32_t iIndex = pDummyPool->GetObjectIndexSafe(pEntity);
        if (iIndex != -1)
            return pDummyPool->IsContains(iIndex);
    }

    CPoolSAInterface<CObjectSAInterface>* pObjectPool = *reinterpret_cast<CPoolSAInterface<CObjectSAInterface>**>(CLASS_CObjectPool);
    if (pObjectPool)
    {
        std::int32_t iIndex = pObjectPool->GetObjectIndexSafe(reinterpret_cast<CObjectSAInterface*>(pEntity));
        if (iIndex != -1)
            return pObjectPool->IsContains(iIndex);
    }

    return false;
}

static bool IsExpectedRemovalModel(CEntitySAInterface* pEntity, uint16_t usExpectedModel)
{
    if (!pEntity)
        return false;

    const int iModelIndex = pEntity->m_nModelIndex;
    if (iModelIndex < 0 || iModelIndex > 0xFFFF)
        return false;

    return static_cast<uint16_t>(iModelIndex) == usExpectedModel;
}

static void LogStaleTrackedEntity(const char* szContext, CEntitySAInterface* pEntity, const char* szReason, int& iLogCount, bool& bSuppressed)
{
    constexpr int kMaxStaleLogsPerCall = 32;
    if (iLogCount < kMaxStaleLogsPerCall)
    {
        AddReportLog(5562, SString("%s: stale entity 0x%08x skipped (%s)", szContext, pEntity, szReason), 10);
        ++iLogCount;
    }
    else if (!bSuppressed)
    {
        AddReportLog(5562, SString("%s: additional stale entities suppressed this call", szContext), 10);
        bSuppressed = true;
    }
}

CBuildingRemovalSA::CBuildingRemovalSA()
{
    m_pBuildingRemovals = new std::multimap<uint16_t, SBuildingRemoval*>;
    m_pDataBuildings = new std::multimap<uint16_t, sDataBuildingRemovalItem*>;
    m_pBinaryBuildings = new std::multimap<uint16_t, sBuildingRemovalItem*>;
}

void CBuildingRemovalSA::ClearEntityTracking(CEntitySAInterface* pInterface)
{
    if (!pInterface)
        return;

    const DWORD dwEntityKey = reinterpret_cast<DWORD>(pInterface);
    m_pRemovedEntities.erase(dwEntityKey);
    m_pAddedEntities.erase(dwEntityKey);
}

void CBuildingRemovalSA::RemoveBuilding(uint16_t usModelToRemove, float fRange, float fX, float fY, float fZ, char cInterior, size_t* pOutAmount)
{
    // New building Removal
    SBuildingRemoval* pRemoval = new SBuildingRemoval();
    pRemoval->m_usModel = usModelToRemove;
    pRemoval->m_vecPos.fX = fX;
    pRemoval->m_vecPos.fY = fY;
    pRemoval->m_vecPos.fZ = fZ;
    pRemoval->m_fRadius = fRange;
    pRemoval->m_cInterior = cInterior;
    // Push it to the back of the removal list
    m_pBuildingRemovals->insert(std::pair<uint16_t, SBuildingRemoval*>(usModelToRemove, pRemoval));

    bool bFound = false;
    uint uiAmount = 0;
    // Init loop variables
    std::pair<std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator> iterators =
        m_pDataBuildings->equal_range(usModelToRemove);
    std::multimap<uint16_t, sDataBuildingRemovalItem*>::const_iterator iter = iterators.first;
    for (; iter != iterators.second; ++iter)
    {
        sDataBuildingRemovalItem* pFind = (*iter).second;
        if (pFind)
        {
            // if the count is <= 0 and the interface is valid check the distance in case we found a removal (count is used to store if we have already removed
            // this once)
            if (pFind->m_iCount <= 0 && pFind->m_pInterface)
            {
                // Grab distances across each axis
                float fDistanceX = fX - pFind->m_pInterface->m_transform.m_translate.fX;
                float fDistanceY = fY - pFind->m_pInterface->m_transform.m_translate.fY;
                float fDistanceZ = fZ - pFind->m_pInterface->m_transform.m_translate.fZ;

                if (pFind->m_pInterface->matrix != NULL)
                {
                    fDistanceX = fX - pFind->m_pInterface->matrix->vPos.fX;
                    fDistanceY = fY - pFind->m_pInterface->matrix->vPos.fY;
                    fDistanceZ = fZ - pFind->m_pInterface->matrix->vPos.fZ;
                }

                // Square root 'em
                float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);

                // Is it in range
                if (fDistance <= fRange && (pFind->m_pInterface->m_areaCode == cInterior || cInterior == -1))
                {
                    CEntitySAInterface* pInterface = pFind->m_pInterface;
                    // while ( pInterface && pInterface != NULL )
                    // if the interface is valid
                    if (pInterface && pInterface != NULL)
                    {
                        // if the building type is dummy or building and it's not already being removed
                        if ((pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY || pInterface->nType == ENTITY_TYPE_OBJECT) &&
                            pInterface->bRemoveFromWorld != 1)
                        {
                            if (!pInterface->IsPlaceableVTBL())
                            {
                                // Add the Data Building to the list
                                pRemoval->AddDataBuilding(pInterface);
                                // Remove the model from the world
                                pGame->GetWorld()->Remove(pInterface, BuildingRemoval2);
                                m_pRemovedEntities[(DWORD)pInterface] = true;
                                bFound = true;
                            }
                        }
                        // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
                        // pInterface = pInterface->m_pLod;
                    }
                    // Set the count.
                    pFind->m_iCount = 1;
                }
            }
        }
    }

    std::pair<std::multimap<uint16_t, sBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sBuildingRemovalItem*>::iterator> iteratorsBinary =
        m_pBinaryBuildings->equal_range(usModelToRemove);
    std::multimap<uint16_t, sBuildingRemovalItem*>::const_iterator iterBinary = iteratorsBinary.first;
    for (; iterBinary != iteratorsBinary.second; ++iterBinary)
    {
        sBuildingRemovalItem* pFindBinary = (*iterBinary).second;
        if (pFindBinary)
        {
            // if the count is <= 0 and the interface is valid check the distance in case we found a removal (count is used to store if we have already removed
            // this once)
            if (pFindBinary->m_iCount <= 0 && pFindBinary->m_pInterface)
            {
                // Grab distances across each axis
                float fDistanceX = fX - pFindBinary->m_pInterface->m_transform.m_translate.fX;
                float fDistanceY = fY - pFindBinary->m_pInterface->m_transform.m_translate.fY;
                float fDistanceZ = fZ - pFindBinary->m_pInterface->m_transform.m_translate.fZ;

                if (pFindBinary->m_pInterface->matrix != NULL)
                {
                    fDistanceX = fX - pFindBinary->m_pInterface->matrix->vPos.fX;
                    fDistanceY = fY - pFindBinary->m_pInterface->matrix->vPos.fY;
                    fDistanceZ = fZ - pFindBinary->m_pInterface->matrix->vPos.fZ;
                }

                // Square root 'em
                float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);

                // Is it in range
                if (fDistance <= fRange && (pFindBinary->m_pInterface->m_areaCode == cInterior || cInterior == -1))
                {
                    CEntitySAInterface* pInterface = pFindBinary->m_pInterface;
                    // while ( pInterface && pInterface != NULL )
                    // if the interface is valid
                    if (pInterface && pInterface != NULL)
                    {
                        // if the building type is dummy or building and it's not already being removed
                        if ((pInterface->nType == ENTITY_TYPE_BUILDING || pInterface->nType == ENTITY_TYPE_DUMMY || pInterface->nType == ENTITY_TYPE_OBJECT) &&
                            pInterface->bRemoveFromWorld != 1)
                        {
                            if (!pInterface->IsPlaceableVTBL())
                            {
                                // Add the Data Building to the list
                                pRemoval->AddBinaryBuilding(pInterface);
                                // Remove the model from the world
                                pGame->GetWorld()->Remove(pInterface, BuildingRemoval2);
                                m_pRemovedEntities[(DWORD)pInterface] = true;
                                bFound = true;
                                ++uiAmount;
                            }
                        }
                        // Get next LOD ( LOD's can have LOD's so we keep checking pInterface )
                        // pInterface = pInterface->m_pLod;
                    }
                    // Set the count.
                    pFindBinary->m_iCount = 1;
                }
            }
        }
    }
    if (bFound)
        pGame->GetModelInfo(usModelToRemove)->RestreamIPL();

    if (pOutAmount)
        *pOutAmount = uiAmount;
}

bool CBuildingRemovalSA::RestoreBuilding(uint16_t usModelToRestore, float fRange, float fX, float fY, float fZ, char cInterior, uint* pOutAmount)
{
    bool bSuccess = false;
    uint uiAmount = 0;
    int  iStaleLogCount = 0;
    bool bStaleLogSuppressed = false;

    // Init some variables
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(usModelToRestore);
    std::multimap<uint16_t, SBuildingRemoval*>::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for (; iter != iterators.second;)
    {
        SBuildingRemoval* pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if (pFind)
        {
            // Grab distances across each axis
            float fDistanceX = fX - pFind->m_vecPos.fX;
            float fDistanceY = fY - pFind->m_vecPos.fY;
            float fDistanceZ = fZ - pFind->m_vecPos.fZ;

            // Square root 'em
            float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
            if (fDistance <= pFind->m_fRadius && (cInterior == -1 || pFind->m_cInterior == cInterior))
            {
                // Init some variables
                CEntitySAInterface*                            pEntity = NULL;
                std::list<CEntitySAInterface*>::const_iterator entityIter = pFind->m_pBinaryRemoveList->begin();
                if (pFind->m_pBinaryRemoveList->empty() == false)
                {
                    // Loop through the Binary object list
                    for (; entityIter != pFind->m_pBinaryRemoveList->end();)
                    {
                        // Grab the pEntity
                        pEntity = (*entityIter);
                        // if it's valid re-add it to the world.
                        if (pEntity != NULL)
                        {
                            // Remove it from the binary list
                            pFind->m_pBinaryRemoveList->erase(entityIter++);

                            // Skip if pool slot was freed or reused with a different model
                            if (!IsEntityPoolSlotOccupied(pEntity))
                            {
                                LogStaleTrackedEntity("RestoreBuilding", pEntity, "pool slot freed", iStaleLogCount, bStaleLogSuppressed);
                                ClearEntityTracking(pEntity);
                            }
                            else if (!IsExpectedRemovalModel(pEntity, pFind->m_usModel))
                            {
                                LogStaleTrackedEntity("RestoreBuilding", pEntity, "model mismatch", iStaleLogCount, bStaleLogSuppressed);
                                ClearEntityTracking(pEntity);
                            }
                            // if the building type is dummy or building and it's not already being removed
                            else if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
                                     pEntity->bRemoveFromWorld != 1)
                            {
                                // Don't call this on entities being removed.
                                if (!pEntity->IsPlaceableVTBL())
                                {
                                    pGame->GetWorld()->Add(pEntity, Building_Restore);
                                    m_pRemovedEntities[(DWORD)pEntity] = false;
                                    // If the building isn't streamed in, we won't find the building in the list (because the hook wasn't called) ->
                                    // removeWorldModel doesn't work next time
                                    AddBinaryBuilding(pEntity);
                                    ++uiAmount;
                                }
                            }
                        }
                        else
                            ++entityIter;
                    }
                }
                // Start the iterator with the data remove list first item
                entityIter = pFind->m_pDataRemoveList->begin();
                if (pFind->m_pDataRemoveList->empty() == false)
                {
                    // Loop through the Data list
                    for (; entityIter != pFind->m_pDataRemoveList->end();)
                    {
                        // Grab the pEntity
                        pEntity = (*entityIter);
                        // if it's valid re-add it to the world.
                        if (pEntity != NULL)
                        {
                            pFind->m_pDataRemoveList->erase(entityIter++);

                            // Skip if pool slot was freed or reused with a different model
                            if (!IsEntityPoolSlotOccupied(pEntity))
                            {
                                LogStaleTrackedEntity("RestoreBuilding", pEntity, "pool slot freed", iStaleLogCount, bStaleLogSuppressed);
                                ClearEntityTracking(pEntity);
                            }
                            else if (!IsExpectedRemovalModel(pEntity, pFind->m_usModel))
                            {
                                LogStaleTrackedEntity("RestoreBuilding", pEntity, "model mismatch", iStaleLogCount, bStaleLogSuppressed);
                                ClearEntityTracking(pEntity);
                            }
                            // if the building type is dummy or building and it's not already being removed
                            else if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
                                     pEntity->bRemoveFromWorld != 1)
                            {
                                if (!pEntity->IsPlaceableVTBL())
                                {
                                    pGame->GetWorld()->Add(pEntity, Building_Restore2);
                                    m_pRemovedEntities[(DWORD)pEntity] = false;
                                }
                            }
                        }
                        else
                        {
                            ++entityIter;
                        }
                    }
                }
                // Remove the building from the list
                m_pBuildingRemovals->erase(iter++);
                delete pFind;
                // Success! don't return incase there are any others to delete
                bSuccess = true;
            }
            else
                iter++;
        }
        else
            iter++;
    }
    std::pair<std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator>
                                                                       dataBuildingIterators = m_pDataBuildings->equal_range(usModelToRestore);
    std::multimap<uint16_t, sDataBuildingRemovalItem*>::const_iterator iterator = dataBuildingIterators.first;
    for (; iterator != dataBuildingIterators.second; ++iterator)
    {
        sDataBuildingRemovalItem* pFound = (*iterator).second;
        if (pFound)
        {
            // Grab distances across each axis
            float fDistanceX = fX - pFound->m_pInterface->m_transform.m_translate.fX;
            float fDistanceY = fY - pFound->m_pInterface->m_transform.m_translate.fY;
            float fDistanceZ = fZ - pFound->m_pInterface->m_transform.m_translate.fZ;

            if (pFound->m_pInterface->matrix != NULL)
            {
                fDistanceX = fX - pFound->m_pInterface->matrix->vPos.fX;
                fDistanceY = fY - pFound->m_pInterface->matrix->vPos.fY;
                fDistanceZ = fZ - pFound->m_pInterface->matrix->vPos.fZ;
            }

            // Square root 'em
            float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
            if (fDistance <= fRange && (cInterior == -1 || pFound->m_pInterface->m_areaCode == cInterior))
            {
                // Fix the removed count.
                pFound->m_iCount--;
                if (pFound->m_iCount < 0)
                    pFound->m_iCount = 0;
            }
        }
    }

    std::pair<std::multimap<uint16_t, sBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sBuildingRemovalItem*>::iterator> binaryBuildingIterators =
        m_pBinaryBuildings->equal_range(usModelToRestore);
    std::multimap<uint16_t, sBuildingRemovalItem*>::const_iterator iteratorBinary = binaryBuildingIterators.first;
    for (; iteratorBinary != binaryBuildingIterators.second; ++iteratorBinary)
    {
        sBuildingRemovalItem* pFoundBinary = (*iteratorBinary).second;
        if (pFoundBinary)
        {
            // Grab distances across each axis
            float fDistanceX = fX - pFoundBinary->m_pInterface->m_transform.m_translate.fX;
            float fDistanceY = fY - pFoundBinary->m_pInterface->m_transform.m_translate.fY;
            float fDistanceZ = fZ - pFoundBinary->m_pInterface->m_transform.m_translate.fZ;

            if (pFoundBinary->m_pInterface->matrix != NULL)
            {
                fDistanceX = fX - pFoundBinary->m_pInterface->matrix->vPos.fX;
                fDistanceY = fY - pFoundBinary->m_pInterface->matrix->vPos.fY;
                fDistanceZ = fZ - pFoundBinary->m_pInterface->matrix->vPos.fZ;
            }

            // Square root 'em
            float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
            if (fDistance <= fRange && (cInterior == -1 || pFoundBinary->m_pInterface->m_areaCode == cInterior))
            {
                // Fix the removed count.
                pFoundBinary->m_iCount--;
                if (pFoundBinary->m_iCount < 0)
                    pFoundBinary->m_iCount = 0;
            }
        }
    }

    if (pOutAmount)
        *pOutAmount = uiAmount;

    return bSuccess;
}

// Check Distance to see if the model being requested is in the radius
bool CBuildingRemovalSA::IsRemovedModelInRadius(SIPLInst* pInst)
{
    // Init some variables
    const int modelIndex = pInst->m_nModelIndex;
    if (modelIndex < 0 || modelIndex > 0xFFFF)
        return false;

    const uint16_t usModelIndex = static_cast<uint16_t>(modelIndex);
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(usModelIndex);
    std::multimap<uint16_t, SBuildingRemoval*>::const_iterator iter = iterators.first;

    // Loop through the buildings list
    for (; iter != iterators.second; ++iter)
    {
        SBuildingRemoval* pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if (pFind)
        {
            // Grab the distance
            float fDistanceX = pFind->m_vecPos.fX - pInst->m_pPosition.fX;
            float fDistanceY = pFind->m_vecPos.fY - pInst->m_pPosition.fY;
            float fDistanceZ = pFind->m_vecPos.fZ - pInst->m_pPosition.fZ;

            float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
            // is it in the removal spheres radius if so return else keep looking
            if (fDistance <= pFind->m_fRadius && (pFind->m_cInterior == -1 || pFind->m_cInterior == pInst->m_nAreaCode))
            {
                return true;
            }
        }
    }
    return false;
}

// Check Distance to see if the model being requested is in the radius
bool CBuildingRemovalSA::IsObjectRemoved(CEntitySAInterface* pInterface)
{
    // Init some variables
    const int modelIndex = pInterface->m_nModelIndex;
    if (modelIndex < 0 || modelIndex > 0xFFFF)
        return false;

    const uint16_t usModelIndex = static_cast<uint16_t>(modelIndex);
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(usModelIndex);
    std::multimap<uint16_t, SBuildingRemoval*>::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for (; iter != iterators.second; ++iter)
    {
        SBuildingRemoval* pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if (pFind)
        {
            // Are we using the interior param?? if so check for a match
            if (pFind->m_cInterior == -1 || pFind->m_cInterior == pInterface->m_areaCode)
            {
                // Grab the distance
                float fDistanceX = pFind->m_vecPos.fX - pInterface->m_transform.m_translate.fX;
                float fDistanceY = pFind->m_vecPos.fY - pInterface->m_transform.m_translate.fY;
                float fDistanceZ = pFind->m_vecPos.fZ - pInterface->m_transform.m_translate.fZ;

                if (pInterface->matrix != NULL)
                {
                    fDistanceX = pFind->m_vecPos.fX - pInterface->matrix->vPos.fX;
                    fDistanceY = pFind->m_vecPos.fY - pInterface->matrix->vPos.fY;
                    fDistanceZ = pFind->m_vecPos.fZ - pInterface->matrix->vPos.fZ;
                }

                float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
                // is it in the removal spheres radius if so return else keep looking
                if (fDistance <= pFind->m_fRadius)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

// Check if a given model is replaced
bool CBuildingRemovalSA::IsModelRemoved(uint16_t usModelID)
{
    return m_pBuildingRemovals->count(usModelID) > 0;
}

// Check if a given model is replaced
bool CBuildingRemovalSA::IsDataModelRemoved(uint16_t usModelID)
{
    return m_pBuildingRemovals->count(usModelID) > 0;
}

// Check if a given model is replaced
bool CBuildingRemovalSA::IsEntityRemoved(CEntitySAInterface* pInterface)
{
    return m_pRemovedEntities.find((DWORD)pInterface) != m_pRemovedEntities.end() && m_pRemovedEntities[(DWORD)pInterface] == true;
}

// Resets deleted list
void CBuildingRemovalSA::ClearRemovedBuildingLists(uint* pOutAmount)
{
    // Ensure no memory leaks by deleting items.
    uint                                                       uiAmount = 0;
    int                                                        iStaleLogCount = 0;
    bool                                                       bStaleLogSuppressed = false;
    std::multimap<uint16_t, SBuildingRemoval*>::const_iterator iter = m_pBuildingRemovals->begin();

    for (; iter != m_pBuildingRemovals->end();)
    {
        SBuildingRemoval* pFind = (*iter).second;
        if (pFind)
        {
            // Init some variables
            CEntitySAInterface*                            pEntity = NULL;
            std::list<CEntitySAInterface*>::const_iterator entityIter = pFind->m_pBinaryRemoveList->begin();
            if (pFind->m_pBinaryRemoveList->empty() == false)
            {
                // Loop through the Binary remove list
                for (; entityIter != pFind->m_pBinaryRemoveList->end(); ++entityIter)
                {
                    // Grab the pEntity
                    pEntity = (*entityIter);
                    // if it's valid re-add it to the world.
                    if (pEntity && pEntity != NULL)
                    {
                        // Skip if pool slot was freed or reused with a different model
                        if (!IsEntityPoolSlotOccupied(pEntity))
                        {
                            ClearEntityTracking(pEntity);
                            LogStaleTrackedEntity("ClearRemovedBuildingLists", pEntity, "pool slot freed", iStaleLogCount, bStaleLogSuppressed);
                        }
                        else if (!IsExpectedRemovalModel(pEntity, pFind->m_usModel))
                        {
                            ClearEntityTracking(pEntity);
                            LogStaleTrackedEntity("ClearRemovedBuildingLists", pEntity, "model mismatch", iStaleLogCount, bStaleLogSuppressed);
                        }
                        // if the building type is dummy or building and it's not already being removed
                        else if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
                                 pEntity->bRemoveFromWorld != 1)
                        {
                            // Don't call this on entities being removed.
                            if (!pEntity->IsPlaceableVTBL())
                            {
                                pGame->GetWorld()->Add(pEntity, BuildingRemovalReset);
                                m_pRemovedEntities[(DWORD)pEntity] = false;
                                ++uiAmount;
                            }
                        }
                    }
                }
            }
            entityIter = pFind->m_pDataRemoveList->begin();
            if (pFind->m_pDataRemoveList->empty() == false)
            {
                // Loop through the Data list
                for (; entityIter != pFind->m_pDataRemoveList->end(); ++entityIter)
                {
                    // Grab the pEntity
                    pEntity = (*entityIter);
                    // if it's valid re-add it to the world.
                    if (pEntity && pEntity != NULL)
                    {
                        // Skip if pool slot was freed or reused with a different model
                        if (!IsEntityPoolSlotOccupied(pEntity))
                        {
                            ClearEntityTracking(pEntity);
                            LogStaleTrackedEntity("ClearRemovedBuildingLists", pEntity, "pool slot freed", iStaleLogCount, bStaleLogSuppressed);
                        }
                        else if (!IsExpectedRemovalModel(pEntity, pFind->m_usModel))
                        {
                            ClearEntityTracking(pEntity);
                            LogStaleTrackedEntity("ClearRemovedBuildingLists", pEntity, "model mismatch", iStaleLogCount, bStaleLogSuppressed);
                        }
                        // if the building type is dummy or building and it's not already being removed
                        else if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
                                 pEntity->bRemoveFromWorld != 1)
                        {
                            // Don't call this on entities being removed.
                            if (!pEntity->IsPlaceableVTBL())
                            {
                                pGame->GetWorld()->Add(pEntity, BuildingRemovalReset2);
                                m_pRemovedEntities[(DWORD)pEntity] = false;
                            }
                        }
                    }
                }
            }
            m_pBuildingRemovals->erase(iter++);
            delete pFind;
        }
        else
            iter++;
    }
    // Init some variables
    std::multimap<uint16_t, sDataBuildingRemovalItem*>::const_iterator iterator = m_pDataBuildings->begin();
    // Loop through the data building list
    for (; iterator != m_pDataBuildings->end(); ++iterator)
    {
        sDataBuildingRemovalItem* pFound = (*iterator).second;
        if (pFound)
        {
            // Set the count to 0 so we can remove it again
            pFound->m_iCount = 0;
        }
    }
    // Init some variables
    std::multimap<uint16_t, sBuildingRemovalItem*>::const_iterator iteratorBinary = m_pBinaryBuildings->begin();
    // Loop through the data building list
    for (; iteratorBinary != m_pBinaryBuildings->end(); ++iteratorBinary)
    {
        sBuildingRemovalItem* pFoundBinary = (*iteratorBinary).second;
        if (pFoundBinary)
        {
            // Set the count to 0 so we can remove it again
            pFoundBinary->m_iCount = 0;
        }
    }
    // Delete old building lists
    delete m_pBuildingRemovals;
    // Create new
    m_pBuildingRemovals = new std::multimap<uint16_t, SBuildingRemoval*>;
    m_pRemovedEntities.clear();
    m_pAddedEntities.clear();

    if (pOutAmount)
        *pOutAmount = uiAmount;
}

// Resets deleted list
SBuildingRemoval* CBuildingRemovalSA::GetBuildingRemoval(CEntitySAInterface* pInterface)
{
    // Init some variables
    const int modelIndex = pInterface->m_nModelIndex;
    if (modelIndex < 0 || modelIndex > 0xFFFF)
        return NULL;

    const uint16_t usModelIndex = static_cast<uint16_t>(modelIndex);
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(usModelIndex);
    std::multimap<uint16_t, SBuildingRemoval*>::const_iterator iter = iterators.first;
    // Loop through the buildings list
    for (; iter != iterators.second; ++iter)
    {
        SBuildingRemoval* pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if (pFind)
        {
            // Grab the distance
            float fDistanceX = pFind->m_vecPos.fX - pInterface->m_transform.m_translate.fX;
            float fDistanceY = pFind->m_vecPos.fY - pInterface->m_transform.m_translate.fY;
            float fDistanceZ = pFind->m_vecPos.fZ - pInterface->m_transform.m_translate.fZ;

            if (pInterface->matrix != NULL)
            {
                fDistanceX = pFind->m_vecPos.fX - pInterface->matrix->vPos.fX;
                fDistanceY = pFind->m_vecPos.fY - pInterface->matrix->vPos.fY;
                fDistanceZ = pFind->m_vecPos.fZ - pInterface->matrix->vPos.fZ;
            }

            float fDistance = sqrt(fDistanceX * fDistanceX + fDistanceY * fDistanceY + fDistanceZ * fDistanceZ);
            // is it in the removal spheres radius if so return else keep looking
            if (fDistance <= pFind->m_fRadius && (pFind->m_cInterior == -1 || pFind->m_cInterior == pInterface->m_areaCode))
            {
                return pFind;
            }
        }
    }
    return nullptr;
}

void CBuildingRemovalSA::AddDataBuilding(CEntitySAInterface* pInterface)
{
    const int modelIndex = pInterface->m_nModelIndex;
    if (modelIndex < 0 || modelIndex > 0xFFFF)
        return;

    const uint16_t usModelIndex = static_cast<uint16_t>(modelIndex);
    if (m_pAddedEntities.find((DWORD)pInterface) == m_pAddedEntities.end() || m_pAddedEntities[(DWORD)pInterface] == false)
    {
        // Create a new building removal
        sDataBuildingRemovalItem* pBuildingRemoval = new sDataBuildingRemovalItem(pInterface, true);
        pBuildingRemoval->m_iplIndex = pInterface->m_iplIndex;  // CEntitySAInterface is forward-decl in the SDK header; m_iplIndex only accessible here
        // Insert it with the model index so we can fast lookup
        m_pDataBuildings->insert(std::pair<uint16_t, sDataBuildingRemovalItem*>(usModelIndex, pBuildingRemoval));
        m_pAddedEntities[(DWORD)pInterface] = true;
        m_pRemovedEntities[(DWORD)pInterface] = false;
    }
}

void CBuildingRemovalSA::AddBinaryBuilding(CEntitySAInterface* pInterface)
{
    const int modelIndex = pInterface->m_nModelIndex;
    if (modelIndex < 0 || modelIndex > 0xFFFF)
        return;

    const uint16_t usModelIndex = static_cast<uint16_t>(modelIndex);
    if (m_pAddedEntities.find((DWORD)pInterface) == m_pAddedEntities.end() || m_pAddedEntities[(DWORD)pInterface] == false)
    {
        // Create a new building removal
        sBuildingRemovalItem* pBuildingRemoval = new sBuildingRemovalItem(pInterface, false);
        pBuildingRemoval->m_iplIndex = pInterface->m_iplIndex;  // CEntitySAInterface is forward-decl in the SDK header; m_iplIndex only accessible here
        // Insert it with the model index so we can fast lookup
        m_pBinaryBuildings->insert(std::pair<uint16_t, sBuildingRemovalItem*>(usModelIndex, pBuildingRemoval));
        m_pAddedEntities[(DWORD)pInterface] = true;
        m_pRemovedEntities[(DWORD)pInterface] = false;
    }
}

void CBuildingRemovalSA::RemoveWorldBuildingFromLists(CEntitySAInterface* pInterface)
{
    const int modelIndex = pInterface->m_nModelIndex;
    if (modelIndex < 0 || modelIndex > 0xFFFF)
        return;

    const uint16_t usModelIndex = static_cast<uint16_t>(modelIndex);
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(usModelIndex);
    std::multimap<uint16_t, SBuildingRemoval*>::const_iterator iter = iterators.first;

    // Loop through the buildings list
    for (; iter != iterators.second; ++iter)
    {
        SBuildingRemoval* pFind = (*iter).second;
        // if pFind is valid and the model is the same
        if (pFind)
        {
            CEntitySAInterface* pEntity = NULL;
            // if the binary remove list is empty don't continue
            if (pFind->m_pBinaryRemoveList->empty() == false)
            {
                // grab the beginning
                std::list<CEntitySAInterface*>::const_iterator entityIter = pFind->m_pBinaryRemoveList->begin();
                // Loop through the binary remove list
                for (; entityIter != pFind->m_pBinaryRemoveList->end();)
                {
                    pEntity = (*entityIter);
                    // is the pointer the same as the one being deleted
                    if ((DWORD)pEntity == (DWORD)pInterface)
                    {
                        // remove it from the binary removed list for this removal
                        pFind->m_pBinaryRemoveList->erase(entityIter++);
                    }
                    else
                        entityIter++;
                }
            }
            if (pFind->m_pDataRemoveList->empty() == false)
            {
                std::list<CEntitySAInterface*>::const_iterator entityIter = pFind->m_pDataRemoveList->begin();
                // Loop through the Data list
                for (; entityIter != pFind->m_pDataRemoveList->end();)
                {
                    // Grab the pEntity
                    pEntity = (*entityIter);
                    // is the pointer the same as the one being deleted
                    if ((DWORD)pEntity == (DWORD)pInterface)
                    {
                        // remove it from the data removed list for this removal
                        pFind->m_pDataRemoveList->erase(entityIter++);
                    }
                    else
                        entityIter++;
                }
            }
        }
    }
    {
        // Init some variables
        std::pair<std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator> dataIterators =
            m_pDataBuildings->equal_range(usModelIndex);
        std::multimap<uint16_t, sDataBuildingRemovalItem*>::const_iterator iterator = dataIterators.first;
        for (; iterator != dataIterators.second;)
        {
            sDataBuildingRemovalItem* pFound = (*iterator).second;
            if (pFound)
            {
                // is the pointer the same as the one being deleted
                if ((DWORD)pFound->m_pInterface == (DWORD)pInterface)
                {
                    delete pFound;
                    // remove it from the data buildings list so we don't try and remove or add it again.
                    m_pDataBuildings->erase(iterator++);
                }
                else
                    iterator++;
            }
            else
                iterator++;
        }
    }
    {
        // Init some variables
        std::pair<std::multimap<uint16_t, sBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sBuildingRemovalItem*>::iterator> binaryIterators =
            m_pBinaryBuildings->equal_range(usModelIndex);
        std::multimap<uint16_t, sBuildingRemovalItem*>::const_iterator iteratorBinary = binaryIterators.first;
        for (; iteratorBinary != binaryIterators.second;)
        {
            sBuildingRemovalItem* pFound = (*iteratorBinary).second;
            if (pFound)
            {
                // is the pointer the same as the one being deleted
                if ((DWORD)pFound->m_pInterface == (DWORD)pInterface)
                {
                    delete pFound;
                    // remove it from the data buildings list so we don't try and remove or add it again.
                    m_pBinaryBuildings->erase(iteratorBinary++);
                }
                else
                    iteratorBinary++;
            }
            else
                iteratorBinary++;
        }
    }
    ClearEntityTracking(pInterface);
}

void CBuildingRemovalSA::DropCaches()
{
    m_pRemovedEntities.clear();
    m_pAddedEntities.clear();

    for (auto& it : *m_pBinaryBuildings)
    {
        delete it.second;
    }

    for (auto& it : *m_pDataBuildings)
    {
        delete it.second;
    }

    m_pBinaryBuildings->clear();
    m_pDataBuildings->clear();

    for (auto& pRemoval : *m_pBuildingRemovals)
    {
        pRemoval.second->m_pDataRemoveList->clear();
        pRemoval.second->m_pBinaryRemoveList->clear();
    }
}

void CBuildingRemovalSA::OnRemoveIpl(int iplSlotIndex)
{
    // m_iplIndex is a BYTE, so only slots 0-255 can be matched by IPL index.
    const bool canMatchByIpl = (iplSlotIndex >= 0 && iplSlotIndex <= 255);
    const BYTE iplIndex = canMatchByIpl ? static_cast<BYTE>(iplSlotIndex) : 0;

    // Purge matching entity pointers from all SBuildingRemoval tracked lists
    for (auto& entry : *m_pBuildingRemovals)
    {
        SBuildingRemoval* pRemoval = entry.second;
        if (!pRemoval)
            continue;

        for (auto it = pRemoval->m_pBinaryRemoveList->begin(); it != pRemoval->m_pBinaryRemoveList->end();)
        {
            CEntitySAInterface* pEntity = *it;
            if (!pEntity)
            {
                ++it;
                continue;
            }

            bool bPoolSlotOccupied = IsEntityPoolSlotOccupied(pEntity);
            bool bMatchesIpl = false;
            if (bPoolSlotOccupied)
            {
                if (!IsExpectedRemovalModel(pEntity, pRemoval->m_usModel))
                    bPoolSlotOccupied = false;
                else
                    bMatchesIpl = canMatchByIpl && pEntity->m_iplIndex == iplIndex;
            }

            if (!bPoolSlotOccupied || bMatchesIpl)
            {
                ClearEntityTracking(pEntity);
                it = pRemoval->m_pBinaryRemoveList->erase(it);
            }
            else
                ++it;
        }

        for (auto it = pRemoval->m_pDataRemoveList->begin(); it != pRemoval->m_pDataRemoveList->end();)
        {
            CEntitySAInterface* pEntity = *it;
            if (!pEntity)
            {
                ++it;
                continue;
            }

            bool bPoolSlotOccupied = IsEntityPoolSlotOccupied(pEntity);
            bool bMatchesIpl = false;
            if (bPoolSlotOccupied)
            {
                if (!IsExpectedRemovalModel(pEntity, pRemoval->m_usModel))
                    bPoolSlotOccupied = false;
                else
                    bMatchesIpl = canMatchByIpl && pEntity->m_iplIndex == iplIndex;
            }

            if (!bPoolSlotOccupied || bMatchesIpl)
            {
                ClearEntityTracking(pEntity);
                it = pRemoval->m_pDataRemoveList->erase(it);
            }
            else
                ++it;
        }
    }

    // Purge matching entries from the data buildings catalog
    for (auto it = m_pDataBuildings->begin(); it != m_pDataBuildings->end();)
    {
        sDataBuildingRemovalItem* pItem = it->second;
        CEntitySAInterface*       pEntity = pItem ? pItem->m_pInterface : nullptr;
        if (!pEntity)
        {
            ++it;
            continue;
        }

        // m_iplIndex was cached when the entity was first tracked (entity was live at that
        // point). Skip pool-slot and model checks for entries from a different IPL; those
        // entities remain in the world and are visited when their own IPL unloads.
        if (canMatchByIpl && pItem->m_iplIndex != iplIndex)
        {
            ++it;
            continue;
        }

        bool bPoolSlotOccupied = IsEntityPoolSlotOccupied(pEntity);
        bool bMatchesIpl = false;
        if (bPoolSlotOccupied)
        {
            if (!IsExpectedRemovalModel(pEntity, it->first))
                bPoolSlotOccupied = false;
            else
                bMatchesIpl = canMatchByIpl && pEntity->m_iplIndex == iplIndex;
        }

        if (!bPoolSlotOccupied || bMatchesIpl)
        {
            ClearEntityTracking(pEntity);
            delete pItem;
            it = m_pDataBuildings->erase(it);
        }
        else
            ++it;
    }

    // Purge matching entries from the binary buildings catalog
    for (auto it = m_pBinaryBuildings->begin(); it != m_pBinaryBuildings->end();)
    {
        sBuildingRemovalItem* pItem = it->second;
        CEntitySAInterface*   pEntity = pItem ? pItem->m_pInterface : nullptr;
        if (!pEntity)
        {
            ++it;
            continue;
        }

        // Same m_iplIndex early-out as the data buildings loop above.
        if (canMatchByIpl && pItem->m_iplIndex != iplIndex)
        {
            ++it;
            continue;
        }

        bool bPoolSlotOccupied = IsEntityPoolSlotOccupied(pEntity);
        bool bMatchesIpl = false;
        if (bPoolSlotOccupied)
        {
            if (!IsExpectedRemovalModel(pEntity, it->first))
                bPoolSlotOccupied = false;
            else
                bMatchesIpl = canMatchByIpl && pEntity->m_iplIndex == iplIndex;
        }

        if (!bPoolSlotOccupied || bMatchesIpl)
        {
            ClearEntityTracking(pEntity);
            delete pItem;
            it = m_pBinaryBuildings->erase(it);
        }
        else
            ++it;
    }
}

// Pre-hook for CIplStore::RemoveIpl (0x404B20).
// When GTA streams out an IPL sector, it frees all entity pool slots in that sector.
// We hook in before that happens to purge tracked pointers that are about to become stale.

#define HOOKPOS_CIplStore_RemoveIpl  0x404B20
#define HOOKSIZE_CIplStore_RemoveIpl 5
static DWORD RETURN_CIplStore_RemoveIpl = 0x404B25;

#define VAR_CIplStore_msPool 0x8E3FB0

static void __cdecl OnIplRemovePre(int iplSlotIndex)
{
    if (!pGame)
        return;

    auto* pBuildingRemoval = static_cast<CBuildingRemovalSA*>(pGame->GetBuildingRemoval());
    if (pBuildingRemoval)
        pBuildingRemoval->OnRemoveIpl(iplSlotIndex);
}

static void __declspec(naked) HOOK_CIplStore_RemoveIpl()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4]          // iplSlotIndex (past 8 pushad regs + return address)
        call    OnIplRemovePre
        add     esp, 4
        popad

        // Overwritten instruction: mov eax, ds:[CIplStore::ms_pPool]
        mov     eax, ds:[VAR_CIplStore_msPool]
        jmp     RETURN_CIplStore_RemoveIpl
    }
    // clang-format on
}

void CBuildingRemovalSA::StaticSetHooks()
{
    static bool bHooksInstalled = false;
    if (bHooksInstalled)
        return;

    EZHookInstall(CIplStore_RemoveIpl);
    bHooksInstalled = true;
}
