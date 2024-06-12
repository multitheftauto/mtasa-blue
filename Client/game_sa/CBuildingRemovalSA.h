/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CBuildingRemovalSA.h
 *  PURPOSE:     Header file for building removal class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <CVector.h>
#include <CVector2D.h>

#include <game/CBuildingRemoval.h>

class CBuildingRemovalSA : public CBuildingRemoval
{
public:
    CBuildingRemovalSA();

    SBuildingRemoval* GetBuildingRemoval(CEntitySAInterface* pInterface) override;

    void RemoveBuilding(uint16_t usModelToRemove, float fDistance, float fX, float fY, float fZ, char cInterior, size_t* pOutAmount = NULL) override;
    bool RestoreBuilding(uint16_t usModelToRestore, float fDistance, float fX, float fY, float fZ, char cInterior, size_t* pOutAmount = NULL) override;
    bool IsRemovedModelInRadius(SIPLInst* pInst) override;
    bool IsModelRemoved(uint16_t modelID) override;
    void ClearRemovedBuildingLists(uint* pOutAmount = NULL) override;
    void AddDataBuilding(CEntitySAInterface* pInterface) override;
    void RemoveWorldBuildingFromLists(CEntitySAInterface* pInterface) override;
    void AddBinaryBuilding(CEntitySAInterface* pInterface) override;
    bool IsObjectRemoved(CEntitySAInterface* pInterface) override;
    bool IsDataModelRemoved(uint16_t usModelID) override;
    bool IsEntityRemoved(CEntitySAInterface* pInterface) override;
    void DropCaches();            

private:
    std::multimap<uint16_t, SBuildingRemoval*>*               m_pBuildingRemovals;
    std::multimap<uint16_t, sDataBuildingRemovalItem*>*       m_pDataBuildings;
    std::multimap<uint16_t, sBuildingRemovalItem*>*           m_pBinaryBuildings;
    std::map<DWORD, bool>                                     m_pRemovedEntities;
    std::map<DWORD, bool>                                     m_pAddedEntities;
};
