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

extern CGameSA* pGame;

CBuildingRemovalSA::CBuildingRemovalSA()
{
    m_pBuildingRemovals = new std::multimap<uint16_t, SBuildingRemoval*>;
    m_pDataBuildings = new std::multimap<uint16_t, sDataBuildingRemovalItem*>;
    m_pBinaryBuildings = new std::multimap<uint16_t, sBuildingRemovalItem*>;
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
    std::pair<std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator>
                                                                             iterators = m_pDataBuildings->equal_range(usModelToRemove);
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
                            // if the building type is dummy or building and it's not already being removed
                            if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
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
                            // if the building type is dummy or building and it's not already being removed
                            if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
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

    std::pair<std::multimap<uint16_t, sBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sBuildingRemovalItem*>::iterator>
                                                                         binaryBuildingIterators = m_pBinaryBuildings->equal_range(usModelToRestore);
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
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(pInst->m_nModelIndex);
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
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(pInterface->m_nModelIndex);
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
    uint                                                             uiAmount = 0;
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
                        // if the building type is dummy or building and it's not already being removed
                        if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
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
                        // if the building type is dummy or building and it's not already being removed
                        if ((pEntity->nType == ENTITY_TYPE_BUILDING || pEntity->nType == ENTITY_TYPE_DUMMY || pEntity->nType == ENTITY_TYPE_OBJECT) &&
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

    if (pOutAmount)
        *pOutAmount = uiAmount;
}

// Resets deleted list
SBuildingRemoval* CBuildingRemovalSA::GetBuildingRemoval(CEntitySAInterface* pInterface)
{
    // Init some variables
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(pInterface->m_nModelIndex);
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
    return NULL;
}

void CBuildingRemovalSA::AddDataBuilding(CEntitySAInterface* pInterface)
{
    if (m_pAddedEntities.find((DWORD)pInterface) == m_pAddedEntities.end() || m_pAddedEntities[(DWORD)pInterface] == false)
    {
        // Create a new building removal
        sDataBuildingRemovalItem* pBuildingRemoval = new sDataBuildingRemovalItem(pInterface, true);
        // Insert it with the model index so we can fast lookup
        m_pDataBuildings->insert(std::pair<uint16_t, sDataBuildingRemovalItem*>((uint16_t)pInterface->m_nModelIndex, pBuildingRemoval));
        m_pAddedEntities[(DWORD)pInterface] = true;
        m_pRemovedEntities[(DWORD)pInterface] = false;
    }
}

void CBuildingRemovalSA::AddBinaryBuilding(CEntitySAInterface* pInterface)
{
    if (m_pAddedEntities.find((DWORD)pInterface) == m_pAddedEntities.end() || m_pAddedEntities[(DWORD)pInterface] == false)
    {
        // Create a new building removal
        sBuildingRemovalItem* pBuildingRemoval = new sBuildingRemovalItem(pInterface, false);
        // Insert it with the model index so we can fast lookup
        m_pBinaryBuildings->insert(std::pair<uint16_t, sBuildingRemovalItem*>((uint16_t)pInterface->m_nModelIndex, pBuildingRemoval));
        m_pAddedEntities[(DWORD)pInterface] = true;
        m_pRemovedEntities[(DWORD)pInterface] = false;
    }
}

void CBuildingRemovalSA::RemoveWorldBuildingFromLists(CEntitySAInterface* pInterface)
{
    std::pair<std::multimap<uint16_t, SBuildingRemoval*>::iterator, std::multimap<uint16_t, SBuildingRemoval*>::iterator> iterators =
        m_pBuildingRemovals->equal_range(pInterface->m_nModelIndex);
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
        std::pair<std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sDataBuildingRemovalItem*>::iterator>
                                                                                 dataIterators = m_pDataBuildings->equal_range(pInterface->m_nModelIndex);
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
        std::pair<std::multimap<uint16_t, sBuildingRemovalItem*>::iterator, std::multimap<uint16_t, sBuildingRemovalItem*>::iterator>
                                                                             binaryIterators = m_pBinaryBuildings->equal_range(pInterface->m_nModelIndex);
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
    m_pRemovedEntities[(DWORD)pInterface] = false;
    m_pAddedEntities[(DWORD)pInterface] = false;
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

    for (auto &pRemoval : *m_pBuildingRemovals)
    {
        pRemoval.second->m_pDataRemoveList->clear();
        pRemoval.second->m_pBinaryRemoveList->clear();
    }
}
