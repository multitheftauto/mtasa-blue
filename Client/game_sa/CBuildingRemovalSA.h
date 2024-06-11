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

    void              RemoveBuilding(unsigned short usModelToRemove, float fDistance, float fX, float fY, float fZ, char cInterior, uint* pOutAmount = NULL);
    bool              IsRemovedModelInRadius(SIPLInst* pInst);
    bool              IsModelRemoved(unsigned short modelID);
    void              ClearRemovedBuildingLists(uint* pOutAmount = NULL);
    bool              RestoreBuilding(unsigned short usModelToRestore, float fDistance, float fX, float fY, float fZ, char cInterior, uint* pOutAmount = NULL);
    SBuildingRemoval* GetBuildingRemoval(CEntitySAInterface* pInterface);
    void              AddDataBuilding(CEntitySAInterface* pInterface);
    void              RemoveWorldBuildingFromLists(CEntitySAInterface* pInterface);
    void              AddBinaryBuilding(CEntitySAInterface* pInterface);
    bool              IsObjectRemoved(CEntitySAInterface* pInterface);
    bool              IsDataModelRemoved(unsigned short usModelID);
    bool              IsEntityRemoved(CEntitySAInterface* pInterface);

private:
    std::multimap<unsigned short, SBuildingRemoval*>*         m_pBuildingRemovals;
    std::multimap<unsigned short, sDataBuildingRemovalItem*>* m_pDataBuildings;
    std::multimap<unsigned short, sBuildingRemovalItem*>*     m_pBinaryBuildings;
    std::map<unsigned short, unsigned short>*                 m_pRemovedObjects;
    std::map<DWORD, bool>                                     m_pRemovedEntities;
    std::map<DWORD, bool>                                     m_pAddedEntities;
};
